#include "stdafx.h"
#include "GltfImporter.h"

#include "GLTF.h"

#include <gtkmm/label.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/separator.h>
#include <gtkmm/box.h>

#include <EtCore/FileSystem/FileUtil.h>

#include <EtPipeline/Assets/EditableMeshAsset.h>
#include <EtPipeline/Import/MeshDataContainer.h>

#include <EtEditor/Util/GtkUtil.h>


namespace et {
namespace edit {


//===============
// GLTF Importer
//===============


//--------------------------
// GltfImporter::c-tor
//
GltfImporter::GltfImporter()
	: ImporterBase()
{
	m_SupportedExtensions.push_back("gltf");
	m_SupportedExtensions.push_back("glb");
}

//----------------------------
// GltfImporter::SetupOptions
//
void GltfImporter::SetupOptions(Gtk::Frame* const frame, T_SensitiveFn& sensitiveFn) 
{
	Gtk::Box* const vbox = Gtk::make_managed<Gtk::Box>(Gtk::ORIENTATION_VERTICAL);
	frame->add(*vbox);

	// at least one asset type should be checked for the import buttonns to be active

	// create a checkbox and label combo
	auto makeOptionFn = 
		[vbox](Glib::ustring const& label, bool& option, bool const isSensitive) -> Gtk::CheckButton*
		{
			Gtk::Box* const hbox = Gtk::make_managed<Gtk::Box>(Gtk::ORIENTATION_HORIZONTAL);
			vbox->pack_start(*hbox, false, true, 3u);

			Gtk::CheckButton* const check = Gtk::make_managed<Gtk::CheckButton>();
			check->set_active(option);
			check->set_sensitive(isSensitive);
			check->signal_toggled().connect([&option, check]()
				{
					option = check->get_active();
				});

			hbox->pack_start(*check, false, false, 3u);
			hbox->pack_start(*Gtk::make_managed<Gtk::Label>(label), false, false, 3u);

			hbox->set_sensitive(isSensitive);

			return check;
		};

	vbox->pack_start(*Gtk::make_managed<Gtk::Label>("Asset types"), false, true, 3u);

	std::vector<Gtk::CheckButton*> assetButtons;

	assetButtons.push_back(makeOptionFn("Import Meshes", m_ImportMeshes, true));
	assetButtons.push_back(makeOptionFn("Import Materials", m_ImportMaterials, false));
	assetButtons.push_back(makeOptionFn("Import Textures", m_ImportTextures, false));
	assetButtons.push_back(makeOptionFn("Import Skeletons", m_ImportSkeletons, false));
	assetButtons.push_back(makeOptionFn("Import Animations", m_ImportAnimations, false));
	assetButtons.push_back(makeOptionFn("Import Templates", m_ImportTemplates, false));

	for (Gtk::CheckButton* const checkButton : assetButtons)
	{
		checkButton->signal_toggled().connect([assetButtons, &sensitiveFn]()
			{
				sensitiveFn(std::find_if(assetButtons.cbegin(), assetButtons.cend(), [](Gtk::CheckButton const* const btn)
					{
						return btn->get_active();
					}) != assetButtons.cend());
			});
	}

	vbox->pack_start(*Gtk::make_managed<Gtk::Separator>(Gtk::ORIENTATION_HORIZONTAL), false, true, 3u);
	vbox->pack_start(*Gtk::make_managed<Gtk::Label>("Mesh options"), false, true, 3u);
	makeOptionFn("Calculate Tangent Space", m_CalculateTangentSpace, true);
	makeOptionFn("Remove duplicate vertices", m_RemoveDuplicateVertices, false);
	makeOptionFn("Include Skeletal data", m_IncludeSkeletalData, false);

	vbox->pack_start(*Gtk::make_managed<Gtk::Separator>(Gtk::ORIENTATION_HORIZONTAL), false, true, 3u);
}

//----------------------
// GltfImporter::Import
//
bool GltfImporter::Import(std::vector<uint8> const& importData, std::string const& filePath, std::vector<pl::EditorAssetBase*>& outAssets) const
{
	glTF::glTFAsset glTfAsset;
	if (!glTF::ParseGLTFData(importData, core::FileUtil::ExtractPath(filePath), core::FileUtil::ExtractExtension(filePath), glTfAsset))
	{
		LOG("failed to load the glTF asset", core::LogLevel::Warning);
		return false;
	}

	if (m_ImportMeshes)
	{
		std::vector<pl::MeshDataContainer*> containers;
		if (!glTF::GetMeshContainers(glTfAsset, containers, m_CalculateTangentSpace))
		{
			LOG("failed to construct mesh data containers from glTF", core::LogLevel::Warning);
			return false;
		}

		for (pl::MeshDataContainer* const meshContainer : containers)
		{
			pl::EditableMeshAsset* const editableMeshAsset = new pl::EditableMeshAsset();
			outAssets.push_back(editableMeshAsset);

			render::MeshAsset* const meshAsset = new render::MeshAsset();
			editableMeshAsset->SetAsset(meshAsset);

			pl::EditableMeshAsset::WriteToEtMesh(meshContainer, meshAsset->GetLoadData());
			if (containers.size() == 1u)
			{
				meshAsset->SetName(core::FileUtil::RemoveExtension(core::FileUtil::ExtractName(filePath)) + "." + pl::EditableMeshAsset::s_EtMeshExt);
			}
			else
			{
				meshAsset->SetName(meshContainer->m_Name + "." + pl::EditableMeshAsset::s_EtMeshExt);
			}

			delete meshContainer;
		}

		containers.clear();
	}
	
	return true;
}


} // namespace edit
} // namespace et
