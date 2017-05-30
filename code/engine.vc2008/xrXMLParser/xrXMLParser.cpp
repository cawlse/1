#include "stdafx.h"
#pragma hdrstop

#include "xrXMLParser.h"


XRXMLPARSER_API CXml::CXml()
	:	m_root			(NULL),
		m_pLocalRoot	(NULL)
{}

XRXMLPARSER_API CXml::~CXml()
{
	ClearInternal();
}

void CXml::ClearInternal()
{
	m_Doc.Clear();
}

void ParseFile(const char* path, CMemoryWriter& W, IReader *F, CXml* xml )
{
	string4096	str;

	while (!F->eof()) 
	{
		F->r_string		(str,sizeof(str));

		if (str[0] == '#' && strstr(str,"#include"))
		{
			string256	inc_name;	
			if (_GetItem	(str,1,inc_name,'"'))
			{
				IReader* I = nullptr;
				if(inc_name==strstr(inc_name,"ui\\"))
				{
					shared_str fn	= xml->correct_file_name("ui", strchr(inc_name,'\\')+1);
					string_path		buff;
					strconcat		(sizeof(buff),buff,"ui\\",fn.c_str());
					I 				= FS.r_open(path, buff);
				}

				if(!I) 
					I = FS.r_open(path, inc_name);

				R_ASSERT4(I, "XML file[%s] parsing failed. Can't find include file:[%s]", path, inc_name);
				
				ParseFile(path, W, I, xml);
				FS.r_close	(I);
			}
		}
		else W.w_string(str);
	}
}

void CXml::Load(const char* path_alias, const char* path, const char* _xml_filename)
{
	shared_str fn			= correct_file_name(path, _xml_filename);

	string_path				str;
	xr_sprintf					(str,"%s\\%s", path, *fn);
	return Load				(path_alias, str);
}

//������������� � �������� XML �����
void CXml::Load(const char* path, const char* xml_filename)
{
	xr_strcpy					(m_xml_file_name, xml_filename);
	// Load and parse xml file

	IReader *F				= FS.r_open(path, xml_filename);
	R_ASSERT2				(F,xml_filename);

	CMemoryWriter			W;
	ParseFile				(path, W, F, this);
	W.w_stringZ				("");
	FS.r_close				(F);

	m_Doc.Parse				((const char*)W.pointer());
	R_ASSERT4(!m_Doc.Error(), "XML file:%s value:%s errDescr:%s", m_xml_file_name, (m_Doc.Value(), m_Doc.ErrorDesc()));

	m_root					= m_Doc.FirstChildElement();
}

XML_NODE* CXml::NavigateToNode(XML_NODE* start_node, const char*  path, int node_index)
{
	R_ASSERT3					(start_node && path, "NavigateToNode failed in XML file ",m_xml_file_name);
	XML_NODE*	node			= NULL;
	XML_NODE*	node_parent		= NULL;
	string_path					buf_str;
	VERIFY						(xr_strlen(path)<200);
	buf_str[0]					= 0;
	xr_strcpy						(buf_str, path);

	char seps[]					= ":";
    char *token;
	int tmp						= 0;

    //������� ���� �� ��������� �������
	token = strtok( buf_str, seps );

	if (token)
	{
		node = start_node->FirstChild(token);

		while (tmp++ < node_index && node)
			node = start_node->IterateChildren(token, node);
	}
	
    while(token)
    {
		// Get next token: 
		token = strtok(nullptr, seps);

		if (token)
			if(node) 
			{
				node_parent = node;
				node = node_parent->FirstChild(token);
			}

    }

	return node;
}

XML_NODE* CXml::NavigateToNode(const char* path, int node_index)
{
	return NavigateToNode(GetLocalRoot()?GetLocalRoot():GetRoot(), path, node_index);
}

XML_NODE* CXml::NavigateToNodeWithAttribute(const char* tag_name, const char* attrib_name, const char* attrib_value)
{

	XML_NODE	*root		= GetLocalRoot() ? GetLocalRoot() : GetRoot();
	int			tabsCount	= GetNodesNum(root, tag_name);

	for (int i = 0; i < tabsCount; ++i)
	{
		const char* result = ReadAttrib(root, tag_name, i, attrib_name, "");
		if (result && xr_strcmp(result, attrib_value) == 0)
		{
			return NavigateToNode(root, tag_name, i);
		}
	}
	return NULL;
}


const char* CXml::Read(const char* path, int index, const char* default_str_val)
{
	XML_NODE* node			= NavigateToNode(path, index);
	return					Read(node, default_str_val);
}

const char* CXml::Read(XML_NODE* start_node, const char* path, int index, const char* default_str_val)
{
	XML_NODE* node			= NavigateToNode(start_node, path, index);
	return					Read(node, default_str_val);
}


const char*  CXml::Read(XML_NODE* node, const char* default_str_val)
{
	if(node && node->FirstChild())
	{
		TiXmlText *text			= node->ToText();
		if (text)				
			return text->Value();
		else 
			return				default_str_val;
	}
	return default_str_val;
}

int CXml::ReadInt(XML_NODE* node, int default_int_val)
{
	const char* result_str		= Read(node, nullptr); 

	if(!result_str)
		return				default_int_val;

	return atoi				(result_str);
}

int CXml::ReadInt(const char* path, int index, int default_int_val)
{
	const char* result_str		= Read(path, index, NULL ); 
	if(!result_str)
		return				default_int_val;

	return atoi				(result_str);
}

int CXml::ReadInt(XML_NODE* start_node, const char* path, int index, int default_int_val)
{
	const char* result_str		= Read(start_node, path, index, nullptr ); 
	if(!result_str)
		return				default_int_val;

	return atoi				(result_str);
}

float   CXml::ReadFlt(const char* path, int index,  float default_flt_val)
{
	const char* result_str = Read(path, index, nullptr);
	if (!result_str)
		return				default_flt_val;

	return (float)atof(result_str);
}

float   CXml::ReadFlt(XML_NODE* start_node,  const char* path, int index,  float default_flt_val)
{
	const char* result_str		= Read(start_node, path, index, nullptr ); 
	if(!result_str)
		return				default_flt_val;

	return (float)atof		(result_str);
}

float   CXml::ReadFlt(XML_NODE* node,  float default_flt_val)
{
	const char* result_str		= Read(node, nullptr ); 

	if(result_str==nullptr)
		return				default_flt_val;

	return (float)atof		(result_str);
}

const char* CXml::ReadAttrib(XML_NODE* start_node, const char* path,  int index, 
					const char* attrib, const char*   default_str_val)
{
	XML_NODE* node			= NavigateToNode(start_node, path, index);
	const char* result			= ReadAttrib(node, attrib, default_str_val);

	return					result;
}


const char* CXml::ReadAttrib(const char* path,  int index, 
					const char* attrib, const char*   default_str_val)
{
	XML_NODE* node			= NavigateToNode(path, index);
	const char* result			= ReadAttrib(node, attrib, default_str_val);
	return					result;
}

const char* CXml::ReadAttrib(XML_NODE* node, const char* attrib, const char* default_str_val)
{
	if(node)
	{
/*
		//����������� ������ ref_str, � �� 
		//�� ������ ��������� ������ � return ������ ����� ���������
		shared_str result_str;
*/
		const char* result_str;
		// ������� ���� �� ��������

		TiXmlElement *el = node->ToElement(); 
		
		if(el)
		{
			result_str = el->Attribute(attrib);
			if (result_str)
				return result_str;
		}
	}
	return default_str_val;
}


int CXml::ReadAttribInt(XML_NODE* node, const char* attrib, int default_int_val)
{
	const char* result_str		= ReadAttrib(node, attrib, nullptr); 

	if(!result_str)
		return				default_int_val;

	return atoi				(result_str);
}

int CXml::ReadAttribInt(const char* path, int index, const char* attrib, int default_int_val)
{
	const char* result_str		= ReadAttrib(path, index, attrib, nullptr); 

	if(!result_str)
		return				default_int_val;

	return atoi				(result_str);
}


int CXml::ReadAttribInt(XML_NODE* start_node, const char* path, int index, const char* attrib, int default_int_val)
{
	const char* result_str		= ReadAttrib(start_node, path, index, attrib, nullptr); 

	if(!result_str)
		return				default_int_val;

	return atoi				(result_str);
}

float   CXml::ReadAttribFlt(const char* path,	int index,  const char* attrib, float default_flt_val)
{
	const char* result_str		= ReadAttrib(path, index, attrib, nullptr); 

	if(!result_str)
		return				default_flt_val;

	return (float)atof		(result_str);
}

float   CXml::ReadAttribFlt(XML_NODE* start_node, const char* path, int index,  const char* attrib, float default_flt_val)
{
	const char* result_str		= ReadAttrib(start_node, path, index, attrib, nullptr); 

	if(!result_str)
		return				default_flt_val;

	return (float)atof		(result_str);
}

float   CXml::ReadAttribFlt(XML_NODE* node,	const char* attrib, float default_flt_val)
{
	const char* result_str		= ReadAttrib(node, attrib, nullptr); 

	if(!result_str)
		return				default_flt_val;

	return (float)atof		(result_str);
}

int CXml::GetNodesNum(const char* path, int index, const char*  tag_name)
{
	XML_NODE	*node,
				*root		= GetLocalRoot()?GetLocalRoot():GetRoot();
	if(path)
	{
		node				= NavigateToNode(path, index);

		if(!node) 
			node			= root;
	}
	else
		node = root;
	
	return node ? GetNodesNum(node, tag_name) : 0;
}

int CXml::GetNodesNum(XML_NODE* node, const char*  tag_name)
{
	int result = 0;
	if (node)
	{
		XML_NODE *el;

		if (!tag_name)
			el = node->FirstChild();
		else
			el = node->FirstChild(tag_name);

		while (el)
		{
			++result;
			if (!tag_name)
				el = el->NextSibling();
			else
				el = el->NextSibling(tag_name);
		}
	}
	return result;
}

//���������� �������� �� ��� ��������
XML_NODE* CXml::SearchForAttribute(const char* path, int index, const char* tag_name, const char* attrib, const char* attrib_value_pattern)
{
	XML_NODE* start_node = NavigateToNode(path, index);
	return	SearchForAttribute(start_node, tag_name, attrib, attrib_value_pattern);
}

XML_NODE* CXml::SearchForAttribute(XML_NODE* start_node, const char* tag_name, const char* attrib, const char* attrib_value_pattern)
{
	while (start_node)
	{
		TiXmlElement *el			= start_node->ToElement();
		if (el)
		{
			const char* attribStr		= el->Attribute(attrib);
			const char* valueStr		= el->Value();

			if (attribStr &&  0 == xr_strcmp(attribStr, attrib_value_pattern) &&
				valueStr && 0 == xr_strcmp(valueStr, tag_name))
			{
				return el;
			}
		}

		XML_NODE *newEl				= start_node->FirstChild(tag_name);
		newEl						= SearchForAttribute(newEl, tag_name, attrib, attrib_value_pattern);
		if (newEl)
			return					newEl;

		start_node					= start_node->NextSibling(tag_name);
	}
	return nullptr;
}

#ifdef DEBUG // debug & mixed

const char* CXml::CheckUniqueAttrib (XML_NODE* start_node, const char* tag_name, const char* attrib_name)
{
	m_AttribValues.clear_not_free	();

	int tags_num					= GetNodesNum(start_node, tag_name);

	for (int i = 0; i<tags_num; i++)
	{
		const char* attrib				= ReadAttrib(start_node, tag_name, i, attrib_name, nullptr);
		
		xr_vector<shared_str>::iterator it = std::find(m_AttribValues.begin(), m_AttribValues.end(), attrib);

		 if(m_AttribValues.end() != it) 
			 return	attrib;
		 
		 m_AttribValues.push_back	(attrib);
	}
	return nullptr;
}
#endif

int APIENTRY DllMain( HANDLE hModule, 
                       u32  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH: {
		break;
	}
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}
