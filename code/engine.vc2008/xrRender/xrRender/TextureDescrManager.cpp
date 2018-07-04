#include "stdafx.h"
#pragma hdrstop
#include "TextureDescrManager.h"
#include "ETextureParams.h"

// eye-params
class cl_dt_scaler		: public R_constant_setup {
public:
	float				scale;

	cl_dt_scaler		(float s) : scale(s)	{};
	virtual void setup	(R_constant* C)
	{
		RCache.set_c	(C,scale,scale,scale,1/ps_r_dtex_range);
	}
};

void fix_texture_thm_name(LPSTR fn)
{
	LPSTR _ext = strext(fn);
	if(  _ext					&&
	  (0==stricmp(_ext,".tga")	||
		0==stricmp(_ext,".thm")	||
		0==stricmp(_ext,".dds")	||
		0==stricmp(_ext,".bmp")	||
		0==stricmp(_ext,".ogm")	) )
		*_ext = 0;
}

struct TH_LoadTHM
{
	using map_TD = xr_map<shared_str, CTextureDescrMngr::texture_desc>;
	using map_CS = xr_map<shared_str, cl_dt_scaler*>;

	LPCSTR initial;
	map_TD &s_texture_details;
    static std::mutex s_texture_details_protector;
	map_CS &s_detail_scalers;
    static std::mutex s_detail_scalers_protector;
};

std::mutex TH_LoadTHM::s_texture_details_protector;
std::mutex TH_LoadTHM::s_detail_scalers_protector;

void CTextureDescrMngr::LoadTHMThread(void* args)
{
	TH_LoadTHM* p = (TH_LoadTHM*)args;
	LoadTHM(p->initial, p->s_texture_details, p->s_detail_scalers);
}

void CTextureDescrMngr::LoadTHM(LPCSTR initial, map_TD &s_texture_details, map_CS &s_detail_scalers)
{
	FS_FileSet				flist;
	FS.file_list			(flist, initial, FS_ListFiles, "*.thm");

	STextureParams			tp;
	string_path				fn;
	for(const FS_File &fs_iter: flist)
	{
		FS.update_path		(fn, initial, fs_iter.name.c_str());
		IReader* F			= FS.r_open(fn);
		xr_strcpy			(fn, fs_iter.name.c_str());
		fix_texture_thm_name(fn);

		R_ASSERT			(F->find_chunk(THM_CHUNK_TYPE));
		F->r_u32			();
		tp.Clear			();
		tp.Load				(*F);
		FS.r_close			(F);
		if (STextureParams::ttImage == tp.type || STextureParams::ttTerrain == tp.type || STextureParams::ttNormalMap == tp.type)
		{
            texture_desc    texture_desc;

			if( tp.detail_name.size() && tp.flags.is_any(STextureParams::flDiffuseDetail|STextureParams::flBumpDetail) )
			{
                texture_desc.m_assoc = xr_new<texture_assoc>();
                texture_desc.m_assoc->detail_name = tp.detail_name;

                TH_LoadTHM::s_detail_scalers_protector.lock();
                auto detailScalerIter = s_detail_scalers.find(fn);
                if (detailScalerIter != s_detail_scalers.end())
                {
                    if (cl_dt_scaler* scaler = detailScalerIter->second)
                    {
                        scaler->scale = tp.detail_scale;
                    }
                    else
                    {
                        detailScalerIter->second = xr_new<cl_dt_scaler>(tp.detail_scale);
                    }
                }
                else
                {
                    cl_dt_scaler* scaler = xr_new<cl_dt_scaler>(tp.detail_scale);
                    s_detail_scalers.insert(std::make_pair(fn, scaler));
                }
                TH_LoadTHM::s_detail_scalers_protector.unlock();

                texture_desc.m_assoc->usage = 0;
				
                if (tp.flags.is(STextureParams::flDiffuseDetail))
                {
                    texture_desc.m_assoc->usage |= (1 << 0);
                }

                if (tp.flags.is(STextureParams::flBumpDetail))
                {
                    texture_desc.m_assoc->usage |= (1 << 1);
                }
			}
            texture_desc.m_spec = xr_new<texture_spec>();
            texture_desc.m_spec->m_material = tp.material + tp.material_weight;
            texture_desc.m_spec->m_use_steep_parallax = false;
			
			if(tp.bump_mode==STextureParams::tbmUse)
			{
                texture_desc.m_spec->m_bump_name = tp.bump_name;
			}
			else if (tp.bump_mode==STextureParams::tbmUseParallax)
			{
                texture_desc.m_spec->m_bump_name = tp.bump_name;
                texture_desc.m_spec->m_use_steep_parallax = true;
			}

            TH_LoadTHM::s_texture_details_protector.lock();
            auto textureDescIter = s_texture_details.find(fn);
            if (textureDescIter != s_texture_details.end())
            {
                xr_delete(textureDescIter->second.m_assoc);
                xr_delete(textureDescIter->second.m_spec);
                s_texture_details.erase(textureDescIter);
            }
            s_texture_details.insert(std::make_pair(fn, texture_desc));
            TH_LoadTHM::s_texture_details_protector.unlock();
		}
	}
}

void CTextureDescrMngr::Load()
{
	TH_LoadTHM* lvltex = new TH_LoadTHM({ "$level_textures$", m_texture_details, m_detail_scalers });
	TH_LoadTHM* gtex = new TH_LoadTHM({ "$game_textures$", m_texture_details, m_detail_scalers });
	TH_LoadTHM* lvl = new TH_LoadTHM({ "$level$", m_texture_details, m_detail_scalers });
	thread_spawn(LoadTHMThread, "X-Ray THM Loader 0", 0, lvltex);
	thread_spawn(LoadTHMThread, "X-Ray THM Loader 1", 0, gtex);
	thread_spawn(LoadTHMThread, "X-Ray THM Loader 2", 0, lvl);
	Sleep(5);
//	LoadTHM("$game_textures$");
//	LoadTHM("$level$");
}

void CTextureDescrMngr::UnLoad()
{
	for(auto &it: m_texture_details)
	{
		xr_delete(it.second.m_assoc);
		xr_delete(it.second.m_spec);
	}
	m_texture_details.clear	();
}

CTextureDescrMngr::~CTextureDescrMngr()
{
	map_CS::iterator I = m_detail_scalers.begin();
	map_CS::iterator E = m_detail_scalers.end();

	for(;I!=E;++I)
		xr_delete(I->second);

	m_detail_scalers.clear	();
}

shared_str CTextureDescrMngr::GetBumpName(const shared_str& tex_name) const
{
	map_TD::const_iterator I = m_texture_details.find	(tex_name);
	if (I!=m_texture_details.end())
	{
		if(I->second.m_spec)
		{
			return I->second.m_spec->m_bump_name;
		}	
	}
	return "";
}

BOOL CTextureDescrMngr::UseSteepParallax(const shared_str& tex_name) const
{
	map_TD::const_iterator I = m_texture_details.find	(tex_name);
	if (I!=m_texture_details.end())
	{
		if(I->second.m_spec)
		{
			return I->second.m_spec->m_use_steep_parallax;
		}	
	}
	return FALSE;
}

float CTextureDescrMngr::GetMaterial(const shared_str& tex_name) const
{
	map_TD::const_iterator I = m_texture_details.find	(tex_name);
	if (I!=m_texture_details.end())
	{
		if(I->second.m_spec)
		{
			return I->second.m_spec->m_material;
		}
	}
	return 1.0f;
}

void CTextureDescrMngr::GetTextureUsage	(const shared_str& tex_name, BOOL& bDiffuse, BOOL& bBump) const
{
	map_TD::const_iterator I = m_texture_details.find	(tex_name);
	if (I!=m_texture_details.end())
	{
		if(I->second.m_assoc)
		{
			u8 usage	= I->second.m_assoc->usage;
			bDiffuse	= !!(usage & (1<<0));
			bBump		= !!(usage & (1<<1));
		}	
	}
}

BOOL CTextureDescrMngr::GetDetailTexture(const shared_str& tex_name, LPCSTR& res, R_constant_setup* &CS) const
{
	map_TD::const_iterator I = m_texture_details.find	(tex_name);
	if (I!=m_texture_details.end())
	{
		if(I->second.m_assoc)
		{
            texture_assoc* TA = I->second.m_assoc;
			res	= TA->detail_name.c_str();
			map_CS::const_iterator It2 = m_detail_scalers.find(tex_name);
			CS	= It2==m_detail_scalers.end()?0:It2->second;//TA->cs;
			return TRUE;
		}
	}
	return FALSE;
}

