#include "SceneLoader.h"

#include <bitset>

#include <angelscript/add_on/scriptarray/scriptarray.h>

#include <VolcaniCore/Core/Assert.h>
#include <VolcaniCore/Core/FileUtils.h>
#include <VolcaniCore/Core/List.h>
#include <VolcaniCore/Core/UUID.h>
#include <Magma/Graphics/StereographicCamera.h>
#include <Magma/Graphics/OrthographicCamera.h>

#include <Magma/Core/YAMLSerializer.h>
#include <Magma/Core/BinaryWriter.h>
#include <Magma/Core/BinaryReader.h>
#include <Magma/Script/ScriptClass.h>
#include <Magma/Scene/Component.h>
#include <Magma/Scene/Component.h>

#include <Lava/Core/App.h>
#include <Lava/Types/GridSet.h>
#include <Lava/Types/GridSet3D.h>
#include <Lava/Types/Timer.h>

#include "EditorApp.h"
#include "ScriptManager.h"

#undef near
#undef far

using namespace Magma::ECS;
using namespace Magma::Physics;
using namespace Lava;

namespace Magma {

template<>
Serializer& Serializer::Write(const VolcaniCore::Vertex& value) {
	SetOptions(Serializer::Options::ArrayOneLine);
	BeginSequence();
		Write(value.Position);
		Write(value.Normal);
		Write(value.TexCoord);
	EndSequence();
	return *this;
}

template<>
Serializer& Serializer::Write(const Asset& value) {
	BeginMapping();
		WriteKey("ID").Write((uint64_t)value.ID);
		WriteKey("Type").Write(AssetTypeToString(value.Type));
	EndMapping();
	return *this;
}

}

namespace Magma {

static void DeserializeEntity(YAML::Node entityNode, Scene& scene);
static void SerializeEntity(YAMLSerializer& out, const Entity& entity);

void SceneLoader::EditorLoad(Scene& scene, const std::string& path) {
	YAML::Node file;
	try {
		file = YAML::LoadFile(path);
	}
	catch(YAML::ParserException e) {
		VOLCANICORE_ASSERT_ARGS(false, "Could not load file %s: %s",
								path.c_str(), e.what());
	}
	auto sceneNode = file["Scene"];

	VOLCANICORE_ASSERT(sceneNode);

	scene.Name = sceneNode["Name"].as<std::string>();

	for(auto node : sceneNode["Entities"])
		DeserializeEntity(node["Entity"], scene);
}

void SceneLoader::EditorSave(const Scene& scene, const std::string& path) {
	YAMLSerializer serializer;
	serializer.BeginMapping(); // File

	serializer
	.WriteKey("Scene").BeginMapping()
		.WriteKey("Name").Write(scene.Name);

		serializer.WriteKey("Entities").BeginSequence(); // Entities
		scene.EntityWorld
		.ForEach(
			[&](const Entity& entity)
			{
				serializer.BeginMapping(); // Entity
				SerializeEntity(serializer, entity);
				serializer.EndMapping(); // Entity
			});
		serializer.EndSequence(); // Entities

	serializer.EndMapping(); // Scene

	serializer.EndMapping(); // File

	serializer.Finalize(path);
}

void SaveScript(YAMLSerializer& serializer, Ref<ScriptObject> obj) {
	serializer.WriteKey("Class").Write(obj->GetClass()->Name);
	serializer.WriteKey("Fields").BeginSequence();

	auto* handle = obj->GetHandle();
	for(uint32_t i = 0; i < handle->GetPropertyCount(); i++) {
		ScriptField field = obj->GetProperty(i);
		bool editorField =
			ScriptManager::FieldHasMetadata(
				obj->GetClass()->Name, field.Name, "EditorField");
		if(!editorField)
			continue;

		serializer.BeginMapping()
			.WriteKey("Field").BeginMapping();

		serializer.WriteKey("Name").Write(field.Name);

		if(field.TypeID == asTYPEID_BOOL) {
			serializer
				.WriteKey("Type").Write(std::string("bool"))
				.WriteKey("Value").Write(*field.As<bool>());
		}
		else if(field.TypeID == asTYPEID_INT8) {
			serializer
				.WriteKey("Type").Write(std::string("int8"))
				.WriteKey("Value").Write((int32_t)*field.As<int8_t>());
		}
		else if(field.TypeID == asTYPEID_INT16) {
			serializer
				.WriteKey("Type").Write(std::string("int16"))
				.WriteKey("Value").Write((int32_t)*field.As<int16_t>());
		}
		else if(field.TypeID == asTYPEID_INT32) {
			serializer
				.WriteKey("Type").Write(std::string("int32"))
				.WriteKey("Value").Write(*field.As<int32_t>());
		}
		else if(field.TypeID == asTYPEID_INT64) {
			serializer
				.WriteKey("Type").Write(std::string("int64"))
				.WriteKey("Value").Write(*field.As<int64_t>());
		}
		else if(field.TypeID == asTYPEID_UINT8) {
			serializer
				.WriteKey("Type").Write(std::string("uint8"))
				.WriteKey("Value").Write((uint32_t)*field.As<uint8_t>());
		}
		else if(field.TypeID == asTYPEID_UINT16) {
			serializer
				.WriteKey("Type").Write(std::string("uint16"))
				.WriteKey("Value").Write((uint32_t)*field.As<uint16_t>());
		}
		else if(field.TypeID == asTYPEID_UINT32) {
			serializer
				.WriteKey("Type").Write(std::string("uint32"))
				.WriteKey("Value").Write(*field.As<uint32_t>());
		}
		else if(field.TypeID == asTYPEID_UINT64) {
			serializer
				.WriteKey("Type").Write(std::string("uint64"))
				.WriteKey("Value").Write(*field.As<uint64_t>());
		}
		else if(field.TypeID == asTYPEID_FLOAT) {
			serializer
				.WriteKey("Type").Write(std::string("float"))
				.WriteKey("Value").Write(*field.As<float>());
		}
		else if(field.TypeID == asTYPEID_DOUBLE) {
			serializer
				.WriteKey("Type").Write(std::string("double"))
				.WriteKey("Value").Write(*(float*)field.As<double>());
		}
		else if(std::string(field.Type->GetName()) == "string") {
			serializer
				.WriteKey("Type").Write(std::string("array"))
				.WriteKey("Value").Write(*field.As<std::string>());
		}
		else if(std::string(field.Type->GetName()) == "array") {
			serializer
				.WriteKey("Type").Write(std::string("array"))
				.WriteKey("Value");

			serializer.SetOptions(Serializer::Options::ArrayOneLine);
			serializer
					.BeginSequence();
			auto* data = field.As<CScriptArray>();
			for(uint32_t i = 0; i < data->GetSize(); i++)
				serializer
					.Write(*(uint32_t*)data->At(i));

			serializer
				.EndSequence();
		}
		else if(std::string(field.Type->GetName()) == "Asset") {
			serializer
				.WriteKey("Type").Write(std::string("Asset"))
				.WriteKey("Value").Write(*field.As<Asset>());
		}
		else if(std::string(field.Type->GetName()) == "Vec3") {
			serializer
				.WriteKey("Type").Write(std::string("Vec3"))
				.WriteKey("Value").Write(*field.As<Vec3>());
		}
		else if(std::string(field.Type->GetName()) == "GridSet") {
			serializer
				.WriteKey("Type").Write(std::string("GridSet"))
				.WriteKey("Value").BeginMapping();

			auto* grid = field.As<GridSet>();
			serializer.WriteKey("Width").Write(grid->GetWidth());
			serializer.WriteKey("Height").Write(grid->GetHeight());

			serializer.WriteKey("Data")
				.BeginSequence();
			for(uint32_t y = 0; y < grid->GetHeight(); y++) {
				serializer.SetOptions(Serializer::Options::ArrayOneLine);
				serializer.BeginSequence();
				for(uint32_t x = 0; x < grid->GetWidth(); x++)
					serializer.Write(*grid->At(x, y));
				serializer.EndSequence();
			}
			serializer.EndSequence();

			serializer.EndMapping();
		}
		// Script Type
		else if(field.Is(ScriptQualifier::ScriptObject)) {
			// auto* type = field.Type;
			// for(uint32_t i = 0; i < type->GetPropertyCount(); i++) {
			// 	uint64_t offset;
			// 	type->GetProperty()
			// }
		}

			serializer.EndMapping()
		.EndMapping();
	}

	serializer
		.EndSequence();
}

void SerializeEntity(YAMLSerializer& serializer, const Entity& entity) {
	serializer.WriteKey("Entity").BeginMapping(); // Entity

	serializer.WriteKey("Name").Write(entity.GetName());
	serializer.WriteKey("ID").Write((uint64_t)entity.GetHandle());

	serializer.WriteKey("Components")
	.BeginMapping(); // Components

	if(entity.Has<TagComponent>()) {
		auto& tag = entity.Get<TagComponent>().Tag;

		serializer.WriteKey("TagComponent")
		.BeginMapping()
			.WriteKey("Tag").Write(tag)
		.EndMapping();
	}
	if(entity.Has<CameraComponent>()) {
		auto& camera = entity.Get<CameraComponent>().Cam;
		auto type = camera->GetType();
		auto s = type == Camera::Type::Ortho ? "Orthographic" : "Stereographic";

		serializer.WriteKey("CameraComponent")
		.BeginMapping()
			.WriteKey("Camera").BeginMapping()
			.WriteKey("Type").Write(s);

		if(type == Camera::Type::Stereo)
			serializer
			.WriteKey("VerticalFOV")
			.Write(camera->As<StereographicCamera>()->GetVerticalFOV());
		else if(type == Camera::Type::Ortho)
			serializer
			.WriteKey("Rotation")
			.Write(camera->As<OrthographicCamera>()->GetRotation());

		serializer
			.WriteKey("Position").Write(camera->GetPosition())
			.WriteKey("Direction").Write(camera->GetDirection())
			.WriteKey("ViewportWidth").Write(camera->GetViewportWidth())
			.WriteKey("ViewportHeight").Write(camera->GetViewportHeight())
			.WriteKey("Near").Write(camera->GetNear())
			.WriteKey("Far").Write(camera->GetFar())
		.EndMapping()
		.EndMapping();
	}
	if(entity.Has<TransformComponent>()) {
		const auto& transform = entity.Get<TransformComponent>();

		serializer.WriteKey("TransformComponent")
		.BeginMapping()
			.WriteKey("Transform").BeginMapping()
				.WriteKey("Translation").Write(transform.Translation)
				.WriteKey("Rotation")	.Write(transform.Rotation)
				.WriteKey("Scale")		.Write(transform.Scale)
			.EndMapping()
		.EndMapping(); // TransformComponent
	}
	if(entity.Has<AudioComponent>()) {
		Asset asset = entity.Get<AudioComponent>().AudioAsset;
		serializer.WriteKey("AudioComponent")
		.BeginMapping()
			.WriteKey("AssetID").Write((uint64_t)asset.ID)
		.EndMapping();
	}
	if(entity.Has<MeshComponent>()) {
		Asset meshSource = entity.Get<MeshComponent>().MeshSourceAsset;
		Asset material = entity.Get<MeshComponent>().MaterialAsset;
		serializer.WriteKey("MeshComponent")
		.BeginMapping()
			.WriteKey("MeshSourceID").Write((uint64_t)meshSource.ID)
			.WriteKey("MaterialID").Write((uint64_t)material.ID)
		.EndMapping();
	}
	if(entity.Has<SkyboxComponent>()) {
		Asset asset = entity.Get<SkyboxComponent>().CubemapAsset;
		serializer.WriteKey("SkyboxComponent")
		.BeginMapping()
			.WriteKey("AssetID").Write((uint64_t)asset.ID)
		.EndMapping();
	}
	if(entity.Has<ScriptComponent>()) {
		const auto& comp = entity.Get<ScriptComponent>();
		auto obj = comp.Instance;

		serializer.WriteKey("ScriptComponent")
		.BeginMapping()
			.WriteKey("ModuleID").Write((uint64_t)comp.ModuleAsset.ID);

		if(obj)
			SaveScript(serializer, obj);

		serializer.EndMapping();
	}
	if(entity.Has<RigidBodyComponent>()) {
		auto body = entity.Get<RigidBodyComponent>().Body;
		serializer.WriteKey("RigidBodyComponent")
		.BeginMapping();

		if(body) {
			auto type = body->GetType();
			auto t = type == RigidBody::Type::Static ? "Static" : "Dynamic";

			serializer.WriteKey("Body")
				.BeginMapping()
					.WriteKey("Type").Write(t);

			if(body->HasShape()) {
				std::string shapeType;
				switch(body->GetShape()->GetType()) {
					case Shape::Type::Box:
						shapeType = "Box";
						break;
					case Shape::Type::Sphere:
						shapeType = "Sphere";
						break;
					case Shape::Type::Plane:
						shapeType = "Plane";
						break;
					case Shape::Type::Capsule:
						shapeType = "Capsule";
						break;
					case Shape::Type::Mesh:
						shapeType = "Mesh";
						break;
				}

				serializer.WriteKey("ShapeType").Write(shapeType);
			}

			serializer.EndMapping(); // Body
		}
		serializer.EndMapping(); // RigidBodyComponent
	}
	if(entity.Has<DirectionalLightComponent>()) {
		const auto& light = entity.Get<DirectionalLightComponent>();

		serializer.WriteKey("DirectionalLightComponent")
		.BeginMapping()
			.WriteKey("Light")
			.BeginMapping()
				.WriteKey("Ambient").Write(light.Ambient)
				.WriteKey("Diffuse").Write(light.Diffuse)
				.WriteKey("Specular").Write(light.Specular)
				.WriteKey("Position").Write(light.Position)
				.WriteKey("Direction").Write(light.Direction)
			.EndMapping()
		.EndMapping(); // DirectionalLightComponent
	}
	if(entity.Has<PointLightComponent>()) {
		const auto& light = entity.Get<PointLightComponent>();
		
		serializer.WriteKey("PointLightComponent")
		.BeginMapping()
			.WriteKey("Light")
			.BeginMapping()
				.WriteKey("Ambient").Write(light.Ambient)
				.WriteKey("Diffuse").Write(light.Diffuse)
				.WriteKey("Specular").Write(light.Specular)
				.WriteKey("Position").Write(light.Position)
				.WriteKey("Constant").Write(light.Constant)
				.WriteKey("Linear").Write(light.Linear)
				.WriteKey("Quadratic").Write(light.Quadratic)
				.WriteKey("Bloom").Write(light.Bloom)
			.EndMapping()
		.EndMapping(); // PointLightComponent
	}
	if(entity.Has<SpotlightComponent>()) {
		const auto& light = entity.Get<SpotlightComponent>();
	
		serializer.WriteKey("SpotlightComponent")
		.BeginMapping()
			.WriteKey("Light")
			.BeginMapping()
				.WriteKey("Ambient").Write(light.Ambient)
				.WriteKey("Diffuse").Write(light.Diffuse)
				.WriteKey("Specular").Write(light.Specular)
				.WriteKey("Position").Write(light.Position)
				.WriteKey("Direction").Write(light.Direction)
				.WriteKey("CutoffAngle").Write(light.CutoffAngle)
				.WriteKey("OuterCutoffAngle").Write(light.OuterCutoffAngle)
			.EndMapping()
		.EndMapping(); // SpotlightComponent
	}
	if(entity.Has<ParticleEmitterComponent>()) {
		const auto& system = entity.Get<ParticleEmitterComponent>();

		serializer.WriteKey("ParticleEmitterComponent")
		.BeginMapping()
			.WriteKey("Position").Write(system.Position)
			.WriteKey("MaxParticleCount").Write(system.MaxParticleCount)
			.WriteKey("ParticleLifetime").Write(system.ParticleLifetime)
			.WriteKey("SpawnInterval").Write(system.SpawnInterval)
			.WriteKey("Offset").Write(system.Offset)
			.WriteKey("MaterialID").Write((uint64_t)system.MaterialAsset.ID)
		.EndMapping(); // ParticleEmitterComponent
	}

	serializer.EndMapping(); // Components

	serializer.EndMapping(); // Entity
}

Ref<ScriptObject> LoadScript(Entity entity, Asset asset,
	YAML::Node& scriptComponentNode)
{
	AssetManager::Get()->Load(asset);
	auto mod = AssetManager::Get()->Get<ScriptModule>(asset);
	if(!mod) {
		VOLCANICORE_LOG_INFO(
			"Could not load script module %lu, needed for Entity %lu",
			(uint64_t)asset.ID, (uint64_t)entity.GetHandle());
		return nullptr;
	}

	if(!scriptComponentNode["Class"])
		return nullptr;

	auto className = scriptComponentNode["Class"].as<std::string>();
	auto _class = mod->GetClass(className);
	if(!_class) {
		VOLCANICORE_LOG_INFO(
			"Could not find class '%s' in module %lu, needed for Entity %lu",
			className.c_str(), (uint64_t)asset.ID, (uint64_t)entity.GetHandle());
		return nullptr;
	}

	auto instance = _class->Construct();
	for(auto fieldNode : scriptComponentNode["Fields"]) {
		auto node = fieldNode["Field"];
		YAML::Node value = node["Value"];
		std::string name = node["Name"].as<std::string>();
		std::string type = node["Type"].as<std::string>();
		void* address = instance->GetProperty(name).Data;
		if(!address)
			continue;

		if(type == "bool")
			*(bool*)address = value.as<bool>();
		else if(type == "int8")
			*(int8_t*)address = (int8_t)value.as<int32_t>();
		else if(type == "int16")
			*(int16_t*)address = (int16_t)value.as<int32_t>();
		else if(type == "int32")
			*(int32_t*)address = value.as<int32_t>();
		else if(type == "int64")
			*(int64_t*)address = value.as<int64_t>();
		else if(type == "uint8")
			*(uint8_t*)address = (uint8_t)value.as<uint32_t>();
		else if(type == "uint16")
			*(uint16_t*)address = (uint16_t)value.as<uint32_t>();
		else if(type == "uint32")
			*(uint32_t*)address = value.as<uint32_t>();
		else if(type == "uint64")
			*(uint64_t*)address = value.as<uint64_t>();
		else if(type == "float")
			*(float*)address = value.as<float>();
		else if(type == "double")
			*(double*)address = value.as<float>();
		else if(type == "string")
			*(std::string*)address = value.as<std::string>();
		else if(type == "array") {
			auto data = value.as<List<uint32_t>>();
			for(uint32_t i = 0; i < data.Count(); i++)
				((CScriptArray*)address)->InsertLast(data.At(i));
		}
		else if(type == "Asset") {
			((Asset*)address)->ID = value["ID"].as<uint64_t>();
			((Asset*)address)->Type =
				AssetTypeFromString(value["Type"].as<std::string>());
		}
		else if(type == "Vec3")
			*(Vec3*)address = value.as<Vec3>();
		else if(type == "GridSet") {
			auto* grid = (GridSet*)address;
			auto width = value["Width"].as<uint32_t>();
			auto height = value["Height"].as<uint32_t>();
			grid->Resize(width, height);

			uint32_t x = 0, y = 0;
			for(auto row : value["Data"]) {
				for(auto val : row)
					*grid->At(x++, y) = (uint8_t)val.as<uint32_t>();
				x = 0;
				y++;
			}
		}
	}

	return instance;
}

void DeserializeEntity(YAML::Node entityNode, Scene& scene) {
	uint64_t entityID = entityNode["ID"].as<uint64_t>();
	Entity entity = scene.EntityWorld.AddEntity(entityID);
	auto name = entityNode["Name"].as<std::string>();
	if(name != "" && name.find_first_not_of(' ') != std::string::npos)
		entity.SetName(name);

	auto components = entityNode["Components"];

	auto cameraComponentNode = components["CameraComponent"];
	if(cameraComponentNode) {
		auto cameraNode = cameraComponentNode["Camera"];
		auto pos  = cameraNode["Position"]		.as<glm::vec3>();
		auto dir  = cameraNode["Direction"]		.as<glm::vec3>();
		auto w	  = cameraNode["ViewportWidth"] .as<uint32_t>();
		auto h	  = cameraNode["ViewportHeight"].as<uint32_t>();
		auto near = cameraNode["Near"]			.as<float>();
		auto far  = cameraNode["Far"]			.as<float>();
		auto typeStr = cameraNode["Type"].as<std::string>();
		float fr;
		Camera::Type type;

		if(typeStr == "Stereographic") {
			type = Camera::Type::Stereo;
			fr = cameraNode["VerticalFOV"].as<float>();
		}
		else if(typeStr == "Orthographic") {
			type = Camera::Type::Ortho;
			fr = cameraNode["Rotation"].as<float>();
		}

		auto camera = Camera::Create(type, fr);
		camera->SetPositionDirection(pos, dir);
		camera->SetProjection(near, far);
		camera->Resize(w, h);

		entity.Add<CameraComponent>(camera);
	}

	auto tagComponentNode = components["TagComponent"];
	if(tagComponentNode) {
		auto tag = tagComponentNode["Tag"].as<std::string>();
		entity.Add<TagComponent>(tag);
	}

	auto transformComponentNode = components["TransformComponent"];
	if(transformComponentNode) {
		auto transformNode = transformComponentNode["Transform"];
		entity.Add<TransformComponent>(
			Transform
			{
				.Translation = transformNode["Translation"].as<glm::vec3>(),
				.Rotation	 = transformNode["Rotation"].as<glm::vec3>(),
				.Scale		 = transformNode["Scale"].as<glm::vec3>()
			});
	}

	auto audioComponentNode = components["AudioComponent"];
	if(audioComponentNode) {
		auto id = audioComponentNode["AssetID"].as<uint64_t>();
		entity.Add<AudioComponent>(Asset{ id, AssetType::Audio });
	}

	auto meshComponentNode = components["MeshComponent"];
	if(meshComponentNode) {
		auto sourceID = meshComponentNode["MeshSourceID"].as<uint64_t>();
		auto materialID = meshComponentNode["MaterialID"].as<uint64_t>();
		entity.Add<MeshComponent>(
			Asset{ sourceID, AssetType::Mesh },
			Asset{ materialID, AssetType::Material });
	}

	auto skyboxComponentNode = components["SkyboxComponent"];
	if(skyboxComponentNode) {
		auto id = skyboxComponentNode["AssetID"].as<uint64_t>();
		entity.Add<SkyboxComponent>(Asset{ id, AssetType::Cubemap });
	}

	auto scriptComponentNode = components["ScriptComponent"];
	if(scriptComponentNode) {
		auto id = scriptComponentNode["ModuleID"].as<uint64_t>();
		Asset asset = { id, AssetType::Script };

		if(!id || !AssetManager::Get()->IsValid(asset))
			entity.Add<ScriptComponent>();
		else {
			auto obj =
				LoadScript(entity, asset, scriptComponentNode);
			entity.Add<ScriptComponent>(asset, obj);
		}
	}

	auto rigidBodyComponentNode = components["RigidBodyComponent"];
	if(rigidBodyComponentNode) {
		auto rigidBodyNode = rigidBodyComponentNode["Body"];
		if(rigidBodyNode) {
			auto typeStr	   = rigidBodyNode["Type"].as<std::string>();
			auto shapeTypeStr  = rigidBodyNode["ShapeType"].as<std::string>();

			RigidBody::Type type =
				(typeStr == "Static") ? RigidBody::Type::Static
									  : RigidBody::Type::Dynamic;
			Shape::Type shapeType;
			if(shapeTypeStr == "Box")	  shapeType = Shape::Type::Box;
			if(shapeTypeStr == "Sphere")  shapeType = Shape::Type::Sphere;
			if(shapeTypeStr == "Plane")	  shapeType = Shape::Type::Plane;
			if(shapeTypeStr == "Capsule") shapeType = Shape::Type::Capsule;
			if(shapeTypeStr == "Mesh")	  shapeType = Shape::Type::Mesh;
	
			Ref<RigidBody> body;
			if(shapeType == Shape::Type::Mesh)
				body = RigidBody::Create(type);
			else {
				Ref<Shape> shape = Shape::Create(shapeType);
				body = RigidBody::Create(type, shape);
			}

			entity.Add<RigidBodyComponent>(body);
		}
		else
			entity.Add<RigidBodyComponent>();
	}

	auto directionalLightComponentNode = components["DirectionalLightComponent"];
	if(directionalLightComponentNode) {
		auto lightNode = directionalLightComponentNode["Light"];
		entity.Add<DirectionalLightComponent>(
			lightNode["Ambient"].as<glm::vec3>(),
			lightNode["Diffuse"].as<glm::vec3>(),
			lightNode["Specular"].as<glm::vec3>(),
			lightNode["Position"].as<glm::vec3>(),
			lightNode["Direction"].as<glm::vec3>());
	}

	auto pointLightComponentNode = components["PointLightComponent"];
	if(pointLightComponentNode) {
		auto lightNode = pointLightComponentNode["Light"];
		entity.Add<PointLightComponent>(
			lightNode["Ambient"].as<glm::vec3>(),
			lightNode["Diffuse"].as<glm::vec3>(),
			lightNode["Specular"].as<glm::vec3>(),
			lightNode["Position"].as<glm::vec3>(),
			lightNode["Constant"].as<float>(),
			lightNode["Linear"].as<float>(),
			lightNode["Quadratic"].as<float>(),
			lightNode["Bloom"].as<bool>());
	}

	auto spotlightComponentNode = components["SpotlightComponent"];
	if(spotlightComponentNode) {
		auto lightNode = spotlightComponentNode["Light"];
		entity.Add<SpotlightComponent>(
			lightNode["Ambient"].as<glm::vec3>(),
			lightNode["Diffuse"].as<glm::vec3>(),
			lightNode["Specular"].as<glm::vec3>(),
			lightNode["Position"].as<glm::vec3>(),
			lightNode["Direction"].as<glm::vec3>(),
			lightNode["CutoffAngle"].as<float>(),
			lightNode["OuterCutoffAngle"].as<float>());
	}

	auto particleEmitterComponentNode = components["ParticleEmitterComponent"];
	if(particleEmitterComponentNode) {
		Asset asset
		{
			particleEmitterComponentNode["MaterialID"].as<uint64_t>(),
			AssetType::Material
		};
		entity.Add<ParticleEmitterComponent>(
			particleEmitterComponentNode["Position"].as<glm::vec3>(),
			particleEmitterComponentNode["MaxParticleCount"].as<uint64_t>(),
			particleEmitterComponentNode["ParticleLifetime"].as<float>(),
			particleEmitterComponentNode["SpawnInterval"].as<float>(),
			particleEmitterComponentNode["Offset"].as<float>(),
			asset);

		entity.GetHandle().modified<ParticleEmitterComponent>();
	}
}

}

namespace Magma {

template<>
BinaryWriter& BinaryWriter::WriteObject(const glm::vec3& vec) {
	Write(vec.x);
	Write(vec.y);
	Write(vec.z);
	return *this;
}

template<>
BinaryWriter& BinaryWriter::WriteObject(const CameraComponent& comp) {
	auto camera = comp.Cam;

	Write((uint32_t)camera->GetType());
	if(camera->GetType() == Camera::Type::Stereo)
		Write(camera->As<StereographicCamera>()->GetVerticalFOV());
	else
		Write(camera->As<OrthographicCamera>()->GetRotation());

	Write(camera->GetPosition());
	Write(camera->GetDirection());
	Write(camera->GetViewportWidth());
	Write(camera->GetViewportHeight());
	Write(camera->GetNear());
	Write(camera->GetFar());

	return *this;
}

template<>
BinaryWriter& BinaryWriter::WriteObject(const TagComponent& comp) {
	Write(comp.Tag);
	return *this;
}

template<>
BinaryWriter& BinaryWriter::WriteObject(const TransformComponent& comp) {
	Write(comp.Translation);
	Write(comp.Rotation);
	Write(comp.Scale);
	return *this;
}

template<>
BinaryWriter& BinaryWriter::WriteObject(const AudioComponent& comp) {
	Write((uint64_t)comp.AudioAsset.ID);

	return *this;
}

template<>
BinaryWriter& BinaryWriter::WriteObject(const MeshComponent& comp) {
	Write((uint64_t)comp.MeshSourceAsset.ID);
	Write((uint64_t)comp.MaterialAsset.ID);
	return *this;
}

template<>
BinaryWriter& BinaryWriter::WriteObject(const SkyboxComponent& comp) {
	Write((uint64_t)comp.CubemapAsset.ID);
	return *this;
}

template<>
BinaryWriter& BinaryWriter::WriteObject(const ScriptComponent& comp) {
	auto obj = comp.Instance;
	auto* handle = obj->GetHandle();

	Write((uint64_t)comp.ModuleAsset.ID);
	Write(obj->GetClass()->Name);

	for(uint32_t i = 0; i < handle->GetPropertyCount(); i++) {
		ScriptField field = obj->GetProperty(i);
		bool editorField =
			ScriptManager::FieldHasMetadata(
				comp.Instance->GetClass()->Name, field.Name, "EditorField");
		if(!editorField) {
			Write((int)-1);
			continue;
		}
		Write(field.TypeID);

		std::string typeName;
		if(field.Type)
			typeName = field.Type->GetName();

		if(field.TypeID == asTYPEID_BOOL)
			Write(*field.As<bool>());
		else if(field.TypeID == asTYPEID_INT8)
			Write(*field.As<int8_t>());
		else if(field.TypeID == asTYPEID_INT16)
			Write(*field.As<int16_t>());
		else if(field.TypeID == asTYPEID_INT32)
			Write(*field.As<int32_t>());
		else if(field.TypeID == asTYPEID_INT64)
			Write(*field.As<int64_t>());
		else if(field.TypeID == asTYPEID_UINT8)
			Write(*field.As<uint8_t>());
		else if(field.TypeID == asTYPEID_UINT16)
			Write(*field.As<uint16_t>());
		else if(field.TypeID == asTYPEID_UINT32)
			Write(*field.As<uint32_t>());
		else if(field.TypeID == asTYPEID_UINT64)
			Write(*field.As<uint64_t>());
		else if(field.TypeID == asTYPEID_FLOAT)
			Write(*field.As<float>());
		else if(field.TypeID == asTYPEID_DOUBLE)
			Write(*field.As<double>());
		else if(typeName == "string")
			Write(*field.As<std::string>());
		else if(typeName == "array") {
			auto* array = field.As<CScriptArray>();
			auto subTypeID = array->GetArrayObjectType()->GetSubTypeId();
			auto* subType = ScriptEngine::Get()->GetTypeInfoById(subTypeID);
			uint64_t size = 0;
			if(subType)
				size = subType->GetSize();
			else
				size = ScriptEngine::Get()->GetSizeOfPrimitiveType(subTypeID);

			uint32_t count = array->GetSize();
			Write((uint32_t)count);
			// Works for primitive and POD types
			WriteData(array->GetBuffer(), size * count);
		}
		else if(typeName == "Asset") {
			auto asset = *field.As<Asset>();
			Write((uint64_t)asset.ID);
			Write((uint8_t)asset.Type);
		}
		else if(typeName == "Vec3")
			Write(*field.As<glm::vec3>());
		else if(typeName == "GridSet") {
			auto* grid = field.As<GridSet>();
			Write(grid->GetWidth());
			Write(grid->GetHeight());
			if(*grid)
				WriteData(grid->Get(), grid->GetCount());
		}
	}

	return *this;
}

template<>
BinaryWriter& BinaryWriter::WriteObject(const RigidBodyComponent& comp) {
	auto body = comp.Body;

	Write((uint8_t)body->GetType());
	Write((uint8_t)body->GetShape()->GetType());

	return *this;
}

template<>
BinaryWriter& BinaryWriter::WriteObject(const DirectionalLightComponent& comp) {
	Write(comp.Ambient);
	Write(comp.Diffuse);
	Write(comp.Specular);
	Write(comp.Position);
	Write(comp.Direction);

	return *this;
}

template<>
BinaryWriter& BinaryWriter::WriteObject(const PointLightComponent& comp) {
	Write(comp.Ambient);
	Write(comp.Diffuse);
	Write(comp.Specular);
	Write(comp.Position);
	Write(comp.Constant);
	Write(comp.Linear);
	Write(comp.Quadratic);
	Write(comp.Bloom);

	return *this;
}

template<>
BinaryWriter& BinaryWriter::WriteObject(const SpotlightComponent& comp) {
	Write(comp.Ambient);
	Write(comp.Diffuse);
	Write(comp.Specular);
	Write(comp.Position);
	Write(comp.Direction);
	Write(comp.CutoffAngle);
	Write(comp.OuterCutoffAngle);

	return *this;
}

template<>
BinaryWriter& BinaryWriter::WriteObject(const ParticleEmitterComponent& comp) {
	Write(comp.Position);
	Write(comp.MaxParticleCount);
	Write(comp.ParticleLifetime);
	Write(comp.SpawnInterval);
	Write(comp.Offset);
	Write((uint64_t)comp.MaterialAsset.ID);

	return *this;
}

template<>
BinaryWriter& BinaryWriter::WriteObject(const Entity& entity) {
	Write((uint64_t)entity.GetHandle());
	Write(entity.GetName());

	std::bitset<12> componentBits;
	componentBits |= ((uint16_t)entity.Has<CameraComponent>()			<< 0);
	componentBits |= ((uint16_t)entity.Has<TagComponent>()				<< 1);
	componentBits |= ((uint16_t)entity.Has<TransformComponent>()		<< 2);
	componentBits |= ((uint16_t)entity.Has<AudioComponent>()			<< 3);
	componentBits |= ((uint16_t)entity.Has<MeshComponent>()				<< 4);
	componentBits |= ((uint16_t)entity.Has<SkyboxComponent>()			<< 5);
	componentBits |= ((uint16_t)entity.Has<ScriptComponent>()			<< 6);
	if(entity.Has<ScriptComponent>() && !entity.Get<ScriptComponent>().Instance)
		componentBits.set(6, false);
	componentBits |= ((uint16_t)entity.Has<RigidBodyComponent>()		<< 7);
	componentBits |= ((uint16_t)entity.Has<DirectionalLightComponent>() << 8);
	componentBits |= ((uint16_t)entity.Has<PointLightComponent>()		<< 9);
	componentBits |= ((uint16_t)entity.Has<SpotlightComponent>()		<< 10);
	componentBits |= ((uint16_t)entity.Has<ParticleEmitterComponent>()	<< 11);

	Write((uint16_t)componentBits.to_ulong());

	if(componentBits.test(0))
		Write(entity.Get<CameraComponent>());
	if(componentBits.test(1))
		Write(entity.Get<TagComponent>());
	if(componentBits.test(2))
		Write(entity.Get<TransformComponent>());
	if(componentBits.test(3))
		Write(entity.Get<AudioComponent>());
	if(componentBits.test(4))
		Write(entity.Get<MeshComponent>());
	if(componentBits.test(5))
		Write(entity.Get<SkyboxComponent>());
	if(componentBits.test(6))
		Write(entity.Get<ScriptComponent>());
	if(componentBits.test(7))
		Write(entity.Get<RigidBodyComponent>());
	if(componentBits.test(8))
		Write(entity.Get<DirectionalLightComponent>());
	if(componentBits.test(9))
		Write(entity.Get<PointLightComponent>());
	if(componentBits.test(10))
		Write(entity.Get<SpotlightComponent>());
	if(componentBits.test(11))
		Write(entity.Get<ParticleEmitterComponent>());

	return *this;
}

}

namespace Magma {

void SceneLoader::RuntimeSave(const Scene& scene,
							  const std::string& projectPath,
							  const std::string& exportPath)
{
	namespace fs = std::filesystem;

	auto scenePath =
		(fs::path(projectPath) / "Visual" / "Scene" / scene.Name
		).string() + ".magma.scene";
	auto binPath =
		(fs::path(exportPath) / "Scene" / scene.Name).string() + ".bin";

	BinaryWriter writer(binPath);

	writer.Write(scene.Name);

	uint64_t entityCount = 0;
	uint64_t idx = writer.GetPosition();
	writer.Advance(sizeof(uint64_t));

	scene.EntityWorld
	.ForEach(
		[&](const Entity& entity)
		{
			writer.Write(entity);
			entityCount++;
		});

	writer.SetPosition(idx);
	writer.Write(entityCount);
}

}
