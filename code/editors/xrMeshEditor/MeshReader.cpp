#include "MainForm.h"
#include "xrCore/xrCore.h"
#include "xrCore/fs_internal.h"
#define ECORE_API
#define ENGINE_API
#include "Meshs.h"
#pragma comment(lib, "xrECoreLite.lib")
using namespace ECore;
//#include <string>

#pragma warning(push)
#pragma warning(disable : 4995) // ignore deprecation warnings
#include <msclr/marshal.h>
using msclr::interop::marshal_context;
#pragma warning(pop)

System::Void MeshEdit::MeshEdit_Load(System::Object^  sender, System::EventArgs^  e)
{
	GEMLib.Load();
}

System::Void MeshEdit::gameMaterialsToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
{
	for (u32 it = 0; it < GEMLib.CountMaterial(); it++)
	{
		System::String^ name = gcnew System::String(GEMLib.GetMaterialByIdx(it)->m_Name.c_str());
		this->GMtList->Items->Add(name);
	}
	this->GMtList->Visible = !this->GMtList->Visible;
}

System::Void MeshEdit::BonesList_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e)
{
}

System::Void MeshEdit::bonesListToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
{
	if (mesh && !this->BonesList->Visible)
	{
		for (u32 it = 0; it < mesh->GetBoneSize(); it++)
		{
			System::String^ name = gcnew System::String(mesh->GetBone(it)->Name().c_str());
			this->BonesList->Items->Add(name);
		}
	}
	else if (this->BonesList->Visible)
	{
		this->BonesList->Items->Clear();
	}

	this->BonesList->Visible = !this->BonesList->Visible;
}

System::Void MeshEdit::loadToolStripMenuItem_Click(System::Object^ sender, System::EventArgs^ e)
{
	if (openFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK)
	{
		marshal_context context;
		const char* tmpStr = context.marshal_as<const char*>(openFileDialog1->FileName);
		mesh = new Mesh(tmpStr);
	}
}

System::Void MeshEdit::xRayIngameToolStripMenuItem_Click(System::Object^ sender, System::EventArgs^ e) 
{
	this->saveFileDialog1->FileName = gcnew System::String(mesh->path.c_str());
	this->saveFileDialog1->Filter = L"Stalker Mesh (*.ogf)|*.ogf";

	if (saveFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK)
	{
		marshal_context context;
		mesh->path = context.marshal_as<const char*>(saveFileDialog1->FileName);
		mesh->ExportOgf();
	}
}