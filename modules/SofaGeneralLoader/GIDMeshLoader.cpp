#include <algorithm>
#include <string>

#include <sofa/core/ObjectFactory.h>
#include <SofaGeneralLoader/GIDMeshLoader.h>

namespace sofa
{

namespace component
{

namespace loader
{

SOFA_DECL_CLASS(GIDMeshLoader)

int GIDMeshLoaderClass = core::RegisterObject("Loads volumetric meshes generated by GID. Some element types are not implemented.")
		.add< GIDMeshLoader >();

GIDMeshLoader::GIDMeshLoader() :
	MeshLoader()
{

}

GIDMeshLoader::~GIDMeshLoader()
{
}

bool GIDMeshLoader::load()
{
	std::ifstream file(m_filename.getFullPath().c_str());

	if( !file.good() )
	{
		serr << "Unable to open file " << m_filename.getFullPath() << sendl;
		return false;
	}

	return readGID(file);
}

bool GIDMeshLoader::readGID(std::ifstream &file)
{
	helper::vector<Coord>& vertices = *positions.beginEdit();

	std::string line;
	std::istringstream iss;
	std::getline(file, line);

	iss.str(line);
	std::string finput;

	iss >> finput;

	std::cout << finput << std::endl;
	if( finput == "MESH" )
	{
		if( iss.eof() )
		{
			serr << "Bad GID file" << sendl;
			return false;
		}

		iss >> finput;

		while( finput != "dimension" )
			iss >> finput;

		if( iss.eof() )
		{
			serr << "Bad GID file" << sendl;
			return false;
		}

		if( finput == "dimension" )
		{

			if( !(iss >> m_dimensions) )
			{
				serr << "Bah GID mesh header : missing dimension information" << sendl;
				return false;
			}

			sout << "dimensions = " << m_dimensions << sendl;

			if( iss.eof() )
			{
				serr << "Bad GID file" << sendl;
				return false;
			}

			iss >> finput;

			if( finput == "ElemType" )
			{
				std::string element = "";
				iss >> element;

				if( element == "Linear" )
					m_eltType = LINEAR;

				if( element == "Triangle" )
					m_eltType = TRIANGLE;

				if( element == "Quadrilateral" )
					m_eltType = QUADRILATERAL;

				if( element == "Tetrahedra" )
					m_eltType = TETRAHEDRA;

				if( element == "Hexahedra" )
					m_eltType = HEXAHEDRA;

				sout << "Elemtype = " << element << sendl;

				if( element == "Prism" )
				{
					serr << "Element type Prism is currently unsupported in SOFA" << sendl;
					return false;
				}

				if( element == "Pyramid" )
				{
					serr << "Element type Pyramid is currently unsupported in SOFA" << sendl;
					return false;
				}

				if( element == "Sphere" )
				{
					serr << "Element type Sphere is currently unsupported in SOFA" << sendl;
					return false;
				}

				if( element == "Circle" )
				{
					serr << "Element type Circle is currently unsupported in SOFA" << sendl;
					return false;
				}

				if( element.empty() )
				{
					serr << "Bad GID file header : unknown element type." << sendl;
					return false;
				}

				iss >> finput;

				if( finput == "Nnode" )
				{
					if( !(iss >> m_nNode) )
					{
						serr << "Bad GID file header" << sendl;
						return false;
					}

					sout << "Nnodes = " << m_nNode << sendl;

					if( (m_eltType == LINEAR) && ( m_nNode != 2 && m_nNode != 3 ) )
					{
						serr << "Incompatible node count for element type Linear : expected 2 or 3, found " << m_nNode << "." << sendl;
						return false;
					}

					if( (m_eltType == TRIANGLE) && ( m_nNode != 3 && m_nNode != 6 ) )
					{
						serr << "Incompatible node count for element type Triangle : expected 2 or 3, found " << m_nNode << "." << sendl;
						return false;
					}

					if( (m_eltType == QUADRILATERAL) && ( m_nNode != 4 && m_nNode != 8 && m_nNode != 9 ) )
					{
						serr << "Incompatible node count for element type Quadrilateral : expected 4, 8 or 9, found " << m_nNode << "." << sendl;
						return false;
					}


					if( (m_eltType == TETRAHEDRA) && ( m_nNode != 4 && m_nNode != 10 ) )
					{
						serr << "Incompatible node count for element type Tetrahedra : expected 4 or 10, found " << m_nNode << "." << sendl;
						return false;
					}

					if( (m_eltType == HEXAHEDRA) && ( m_nNode != 8 && m_nNode != 20 && m_nNode != 27 ) )
					{
						serr << "Incompatible node count for element type Quadrilateral : expected 8, 20 or 27, found " << m_nNode << "." << sendl;
						return false;
					}

				}
				else
				{
					serr << "Bad GID file header : expecting Nnode tag." << sendl;
					return false;
				}

			}
			else
			{
				serr << "Bad GID file header : missing Elemtype tag." << sendl;
				return false;
			}
		}
		else
		{
			serr << "Bad GID file header : missing dimension tag." << sendl;
			return false;
		}
	}
	else
	{
		serr << "Bad GID mesh header" << sendl;
		return false;
	}

	do
	{
		std::getline(file, line);
		std::transform(line.begin(), line.end(), line.begin(), ::tolower);
	}
	while( line != "coordinates" );

	std::getline(file, line);
	std::transform(line.begin(), line.end(), line.begin(), ::tolower);

//	std::istringstream test("    1               0              -5               0");
//	int one;
//	double two, three, four;
//	test >> one;
//	test >> two;
//	test >> three;
//	test >> four;

//	std::cout << one << " " << two << " " << three << " " << four << std::endl;

	while( line != "end coordinates")
	{
//		std::cout << line << std::endl;
		int vid;


		iss.str(line);
//		iss.seekg(0, std::ios::beg);
		iss.clear();
		iss >> vid; // vertex id

		Coord vtx;
		for(unsigned char d = 0 ; d < m_dimensions ; ++d)
		{
			iss >> vtx(d);
		}

		vertices.push_back(vtx);

		if( !file.good() )
		{
			serr << "Bad GID file : unexpected EOF.";
			return false;
		}

		getline(file, line);
		std::transform(line.begin(), line.end(), line.begin(), ::tolower);
	}

	positions.endEdit();

	do
	{
		std::getline(file, line);
		std::transform(line.begin(), line.end(), line.begin(), ::tolower);
	}
	while( line != "elements" );

	switch(m_eltType)
	{
	case LINEAR :
		return readLinearElements(file);
		break;

	case TRIANGLE :
		return readTriangleElements(file);
		break;

	case QUADRILATERAL :
		return readQuadrilateralElements(file);
		break;

	case TETRAHEDRA :
		return readTetrahedralElements(file);
		break;

	case HEXAHEDRA :
		return readHexahedralElements(file);
		break;

    default :
		break;
	}

    return false;
}

bool GIDMeshLoader::readLinearElements(std::ifstream &file)
{
	std::string line;
	std::istringstream iss;
	helper::vector<Edge>& meshEdges = *edges.beginEdit();

	std::getline(file, line);
	std::transform(line.begin(), line.end(), line.begin(), ::tolower);

	if( m_nNode != 2 )
		serr << "Warning : implementation only supports 2 nodes Linear elements" << sendl;

	while( line != "end elements")
	{
		unsigned int eid; // element id
		unsigned int vid; // vertex id
		iss.str(line);
		iss.clear();

		iss >> eid;

		Edge e;

		if( !(iss >> vid) )
		{
			serr << "Error reading GID file : expecting node index" << sendl;
			return false;
		}
		e[0] = vid - 1;


		if( !(iss >> vid) )
		{
			serr << "Error reading GID file : expecting node index" << sendl;
			return false;
		}
		e[1] = vid - 1;

		// we do not treat the middle vertex in the case of a 3 node Linear element

		meshEdges.push_back(e);

		if( !file.good() )
		{
			serr << "Bad GID file : unexpected EOF.";
			return false;
		}

		getline(file, line);
		std::transform(line.begin(), line.end(), line.begin(), ::tolower);
	}

	return true;
}

bool GIDMeshLoader::readTriangleElements(std::ifstream &file)
{
	std::string line;
	std::istringstream iss;
	helper::vector<Triangle>& meshTriangles = *triangles.beginEdit();

	std::getline(file, line);
	std::transform(line.begin(), line.end(), line.begin(), ::tolower);

	if( m_nNode != 3 )
		serr << "Warning : implementation only supports 3 nodes Triangle elements" << sendl;

	while( line != "end elements")
	{
		unsigned int eid; // element id
		unsigned int vid; // vertex id
		iss.str(line);
		iss.clear();

		iss >> eid;

		Triangle t;

		if( !(iss >> vid) )
		{
			serr << "Error reading GID file : expecting node index" << sendl;
			return false;
		}
		t[0] = vid - 1;

		if( !(iss >> vid) )
		{
			serr << "Error reading GID file : expecting node index" << sendl;
			return false;
		}
		t[1] = vid - 1;

		if( !(iss >> vid) )
		{
			serr << "Error reading GID file : expecting node index" << sendl;
			return false;
		}
		t[2] = vid - 1;

		meshTriangles.push_back(t);

		if( !file.good() )
		{
			serr << "Bad GID file : unexpected EOF.";
			return false;
		}

		getline(file, line);
		std::transform(line.begin(), line.end(), line.begin(), ::tolower);
	}

	return true;
}

bool GIDMeshLoader::readQuadrilateralElements(std::ifstream &file)
{
	std::string line;
	std::istringstream iss;
	helper::vector<Quad>& meshQuads = *quads.beginEdit();

	std::getline(file, line);
	std::transform(line.begin(), line.end(), line.begin(), ::tolower);

	if( m_nNode != 4 )
		serr << "Warning : implementation only supports 4 nodes Quadrilateral elements" << sendl;

	while( line != "end elements")
	{
		unsigned int eid; // element id
		unsigned int vid; // vertex id
		iss.str(line);
		iss.clear();

		iss >> eid;

		Quad q;

		if( !(iss >> vid) )
		{
			serr << "Error reading GID file : expecting node index" << sendl;
			return false;
		}
		q[0] = vid - 1;

		if( !(iss >> vid) )
		{
			serr << "Error reading GID file : expecting node index" << sendl;
			return false;
		}
		q[1] = vid - 1;

		if( !(iss >> vid) )
		{
			serr << "Error reading GID file : expecting node index" << sendl;
			return false;
		}
		q[2] = vid - 1;

		if( !(iss >> vid) )
		{
			serr << "Error reading GID file : expecting node index" << sendl;
			return false;
		}
		q[3] = vid - 1;


		meshQuads.push_back(q);

		if( !file.good() )
		{
			serr << "Bad GID file : unexpected EOF.";
			return false;
		}

		getline(file, line);
		std::transform(line.begin(), line.end(), line.begin(), ::tolower);
	}

	return true;
}

bool GIDMeshLoader::readTetrahedralElements(std::ifstream &file)
{
	std::string line;
	std::istringstream iss;
	helper::vector<Tetrahedron>& meshTetra = *tetrahedra.beginEdit();

	std::getline(file, line);
	std::transform(line.begin(), line.end(), line.begin(), ::tolower);

	if( m_nNode != 4 )
		serr << "Warning : implementation only supports 4 nodes Tetrahedra elements" << sendl;

	while( line != "end elements")
	{
		unsigned int eid; // element id
		unsigned int vid; // vertex id
		iss.str(line);
		iss.clear();

		iss >> eid;

		Tetrahedron t;

		if( !(iss >> vid) )
		{
			serr << "Error reading GID file : expecting node index" << sendl;
			return false;
		}
		t[0] = vid - 1;

		if( !(iss >> vid) )
		{
			serr << "Error reading GID file : expecting node index" << sendl;
			return false;
		}
		t[1] = vid - 1;

		if( !(iss >> vid) )
		{
			serr << "Error reading GID file : expecting node index" << sendl;
			return false;
		}
		t[2] = vid - 1;

		if( !(iss >> vid) )
		{
			serr << "Error reading GID file : expecting node index" << sendl;
			return false;
		}
		t[3] = vid - 1;


		meshTetra.push_back(t);

		if( !file.good() )
		{
			serr << "Bad GID file : unexpected EOF.";
			return false;
		}

		getline(file, line);
		std::transform(line.begin(), line.end(), line.begin(), ::tolower);
	}

	tetrahedra.endEdit();
	return true;
}

bool GIDMeshLoader::readHexahedralElements(std::ifstream &file)
{
	std::string line;
	std::istringstream iss;
	helper::vector<Hexahedron>& meshHexa = *hexahedra.beginEdit();

	std::getline(file, line);
	std::transform(line.begin(), line.end(), line.begin(), ::tolower);

	if( m_nNode != 8 )
		serr << "Warning : implementation only supports 8 nodes Hexahedra elements" << sendl;

	while( line != "end elements")
	{
		unsigned int eid; // element id
		unsigned int vid; // vertex id
		iss.str(line);
		iss.clear();

		iss >> eid;

		Hexahedron h;

		if( !(iss >> vid) )
		{
			serr << "Error reading GID file : expecting node index" << sendl;
			return false;
		}
		h[0] = vid - 1;

		if( !(iss >> vid) )
		{
			serr << "Error reading GID file : expecting node index" << sendl;
			return false;
		}
		h[1] = vid - 1;

		if( !(iss >> vid) )
		{
			serr << "Error reading GID file : expecting node index" << sendl;
			return false;
		}
		h[2] = vid - 1;

		if( !(iss >> vid) )
		{
			serr << "Error reading GID file : expecting node index" << sendl;
			return false;
		}
		h[3] = vid - 1;

		if( !(iss >> vid) )
		{
			serr << "Error reading GID file : expecting node index" << sendl;
			return false;
		}
		h[4] = vid - 1;

		if( !(iss >> vid) )
		{
			serr << "Error reading GID file : expecting node index" << sendl;
			return false;
		}
		h[5] = vid - 1;

		if( !(iss >> vid) )
		{
			serr << "Error reading GID file : expecting node index" << sendl;
			return false;
		}
		h[6] = vid - 1;

		if( !(iss >> vid) )
		{
			serr << "Error reading GID file : expecting node index" << sendl;
			return false;
		}
		h[7] = vid - 1;


		meshHexa.push_back(h);

		if( !file.good() )
		{
			serr << "Bad GID file : unexpected EOF.";
			return false;
		}

		getline(file, line);
		std::transform(line.begin(), line.end(), line.begin(), ::tolower);
	}

	return true;
}

} // namespace loader

} // namespace component

} // namespace sofa