import geneticalgo
import random
import copy
import os
import accordionutils as accordion
from geneticalgo import algorithm
from geneticalgo import primitives
from geneticalgo import shapewriter
from sofalauncher import launcher

individualId = 0
heightTube = 10.0
radiusTube = 3.0
thickness = 0.5
number_of_cavities=2
generate_random="OFF"
type=["ellipsoid","frisbee"]

mutationType="ON"
mutationAxisX="OFF"
mutationAxisY="OFF"
mutationAxisZ="OFF"

def getNextId():
    global individualId
    individualId+=1
    return individualId


class AccordionIndividual(algorithm.Individual):
    def __init__(self, height, radius, thickness):
        self.level=None
        self.id = getNextId()
        accordion.create(self, height, radius, thickness)
        
    def display(self):
        temp="heightTube="+str(self.height)+"\n"\
             +"radiusTube="+str(self.radius)+"\n"\
             +"thickness="+str(self.thickness)+"\n\n"\

        for i in range(len(self.listCavities)):
            [height,type,axisX,axisY,axisZ]=self.listCavities[i]
            temp=temp+"cavity number "+str(i)+"\n"\
                     +"type="+type+"\n"\
                     +"height="+str(height)+"\n"\
                     +"axisX="+str(axisX)+"\n"\
                     +"axisY="+str(axisY)+"\n"\
                     +"axisZ="+str(axisZ)+"\n\n"
        return temp


def getShapeFromInd(ind):
    return accordion.accordionFreeDimension(ind.height, ind.radius, ind.thickness, ind.listCavities)


#def copyInd (individual):
#    temp=Individual(individual.height,individual.radius,individual.thickness)
#    listCavities=copy.deepcopy(individual.listCavities)
#    temp.listCavities=listCavities
#    return temp


####################################################################################################
### MUTATION
###
def mutation_axisX(ind):
    length=len(ind.listCavities)

    if length==0:
        raise ValueError, "their is no cavity to mutate"

    index=random.randint(0,length-1)
    axisX=ind.listCavities[index][2]
    epsilon=random.uniform(-0.5,0.5)
    axisX=max(4.0, axisX+epsilon)
    ind.listCavities[index][2]=axisX

def mutation_axisY(ind):
    length=len(ind.listCavities)

    if length==0:
        raise ValueError, "their is no cavity to mutate"

    index=random.randint(0,length-1)
    axisY=ind.listCavities[index][3]
    epsilon=random.uniform(-0.5,0.5)
    axisY=max(0.5, axisY+epsilon)
    ind.listCavities[index][3]=axisY


def mutation_axisZ(ind):
    length=len(ind.listCavities)
    if length<=1:
        raise ValueError, "their is no cavity to mutate, or don't touch the first cavity"

    index=random.randint(1,length-1)
    axisZ=ind.listCavities[index][4]
    epsilon=random.uniform(-0.2,0.2)
    axisZ=max(0.5,min(1.5, axisZ+epsilon))
    ind.listCavities[index][4]=axisZ



def mutation_type(ind):
    length=len(ind.listCavities)

    if length==0:
        raise ValueError, "their is no cavity to mutate"

    index=random.randint(0,length-1)
    type=ind.listCavities[index][1]

    if type=="ellipsoid":
        ind.listCavities[index][1]="frisbee"
    else:
        ind.listCavities[index][1]="ellipsoid"


def mutation(ind):
    if mutationAxisX=="ON":
        mutation_axisX(ind)

    if mutationAxisY=="ON":
        mutation_axisY(ind)

    if mutationAxisZ=="ON":
        mutation_axisZ(ind)

    if mutationType=="ON":
        mutation_type(ind)


def mutationFunc(pop, number_of_mutated_ind, number_of_mutation_per_ind):
    length_temp=len(pop)

    for i in range(number_of_mutated_ind):
        j=random.randint(0,length_temp-1)
        ind=copy.deepcopy(pop[j])

        for k in range(number_of_mutation_per_ind):
            mutation(ind)

        pop.append(ind)

    return pop

####################################################################################################
###CROSSING
###
def crossing_ind(individual1, individual2):
    ind1=copy.deepcopy(individual1)
    ind2=copy.deepcopy(individual2)

    index=random.randint(0,number_of_cavities-1)

    temp=ind1.listCavities[index]
    ind1.listCavities[index]=ind2.listCavities[index]
    ind2.listCavities[index]=temp

    return (ind1, ind2)


def crossFunc(pop, number_of_crossing):
    print("crossFunc "+str(pop))

    length_temp=len(pop)

    newpop = algorithm.Population()
    for i in range(number_of_crossing):
        j=random.randint(0, length_temp-1)
        k=random.randint(0, length_temp-1)
        a,b=crossing_ind(pop[j], pop[k])
        newpop.append(a)
        newpop.append(b)
    return newpop

####################################################################################################
### Generate
###
def generateIndividual(aType):
        individual=AccordionIndividual(heightTube, radiusTube, thickness)

        for i in range(1,number_of_cavities+1):
            height=0.5+i*(heightTube-0.5)/float(number_of_cavities)

            if generate_random=="ON":
                axisX=random.uniform(4.0,7.0)
                axisY=random.uniform(1.0,7.0)
                axisZ=(heightTube-0.5)/(2*number_of_cavities)
            else:
                axisX=5.0
                axisY=5.0
                axisZ=(heightTube-0.5)/(2*number_of_cavities)

            cavity=[height,aType,axisX,axisY,axisZ]
            accordion.addCavity(individual, cavity)

        individual.level=random.uniform(0.0,10.0)

        return individual


def generateFunc(numgen, nbInd):
    pop = algorithm.Population()

    for i in range(nbInd):
        ltype=random.choice(type)
        pop.append(generateIndividual(ltype))

    return pop

####################################################################################################
### Eval
###
def evaluationFunc(pop):
    print("Evaluation Function "+str(len(pop)))
    basedir=os.path.dirname(__file__)
    bestscore = 0

    filename=[]
    for ind in pop:
        shape, shapeMinus = getShapeFromInd(ind)

        ### return (shape,shapeMinus)
        ###
        fend =  "def evalField(x,y,z): \n\treturn expression"
        f1 = shapewriter.toPythonString(shape) + fend
        f2 = shapewriter.toPythonString(shapeMinus) + fend

        filename.append((f1,f2, ind))

    #################### EXAMPLE USING THE SEQUENTIAL LAUNCHER #################################
    ### List of filename that contains the simulation to run
    scenefiles = ["scene.pyscn","controller.py", "shape.py", "shapeinv.py"]
    filesandtemplates = []
    for scenefile in scenefiles:
        filesandtemplates.append( (open(basedir+"/"+scenefile).read(), scenefile) )

    for f1, f2, ind in filename:
        runs = []
        for f1,f2,ind in filename:
            runs.append( {"GENERATION": str(pop.id),
                          "INDIVIDUAL": str(ind.id),
                          "SHAPECONTENT": f1, "SHAPEINVCONTENT": f2, "nbIterations":1000 } )

    results = launcher.startSofa(runs, filesandtemplates, launcher=launcher.SerialLauncher())

    for res in results:
               print("Results: ")
               print("    directory: "+res["directory"])
               print("        scene: "+res["scene"])
               print("      logfile: "+res["logfile"])
               print("     duration: "+str(res["duration"])+" sec")

    ### Associate the results to the individuals.
    for i in range(len(filename)):
        f1, f2, ind = filename[i]
        ind.results = results[i]

    ## Wait here the results.
    for f1,f2,ind in filename:
        ind.level = random.uniform(0,random.uniform(0,5))
        if ind.level > bestscore:
            bestscore = ind.level

    return bestscore


####################################################################################################
### Select
###
def selectionFunc(pop):
    print("selectFunc "+str(pop))
    return pop

