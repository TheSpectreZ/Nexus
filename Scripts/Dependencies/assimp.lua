project "assimp"
	kind "SharedLib"
	language "C++"
	location (vcxDir)

	targetdir (BinDir)
    objdir (IntDir)

    includedirs
	{
		(DepDir.."Assimp"),
		(DepDir.."Assimp/include"),
		(DepDir.."Assimp/code"),
		(DepDir.."Assimp/contrib"),
		(DepDir.."Assimp/contrib/irrXML"),
		(DepDir.."Assimp/contrib/unzip"),
		(DepDir.."Assimp/contrib/zlib"),
		(DepDir.."Assimp/contrib/pugixml/src"),
		(DepDir.."Assimp/contrib/rapidjson/include"),
		(DepDir.."Assimp/contrib/openddlparser/include"),
	}

	files
	{
		-- Dependencies
		(DepDir.."Assimp/contrib/unzip/**"),
		(DepDir.."Assimp/contrib/irrXML/**"),
		(DepDir.."Assimp/contrib/zlib/*"),
		
		-- Common
		(DepDir.."Assimp/code/Common/**"),
		(DepDir.."Assimp/code/PostProcessing/**"),
		(DepDir.."Assimp/code/Material/**"),
		(DepDir.."Assimp/code/CApi/**"),
		
		-- Importers
		(DepDir.."Assimp/code/AssetLib/Obj/**"),
		(DepDir.."Assimp/code/AssetLib/FBX/**"),
	}

	defines
	{
		"ASSIMP_BUILD_DLL_EXPORT",

		"_CRT_SECURE_NO_WARNINGS",

		'ASSIMP_BUILD_NO_3D_IMPORTER',
		'ASSIMP_BUILD_NO_3DS_IMPORTER',
		'ASSIMP_BUILD_NO_3MF_IMPORTER',
		'ASSIMP_BUILD_NO_AC_IMPORTER',
		'ASSIMP_BUILD_NO_AMF_IMPORTER',
		'ASSIMP_BUILD_NO_ASE_IMPORTER',
		'ASSIMP_BUILD_NO_ASSBIN_IMPORTER',
		'ASSIMP_BUILD_NO_B3D_IMPORTER',
		'ASSIMP_BUILD_NO_BLEND_IMPORTER',
		'ASSIMP_BUILD_NO_BVH_IMPORTER',
		'ASSIMP_BUILD_NO_C4D_IMPORTER',
		'ASSIMP_BUILD_NO_COB_IMPORTER',
		'ASSIMP_BUILD_NO_COLLADA_IMPORTER',
		'ASSIMP_BUILD_NO_CSM_IMPORTER',
		'ASSIMP_BUILD_NO_DXF_IMPORTER',
		-- 'ASSIMP_BUILD_NO_FBX_IMPORTER',
		'ASSIMP_BUILD_NO_GLTF_IMPORTER',
		'ASSIMP_BUILD_NO_HMP_IMPORTER',
		'ASSIMP_BUILD_NO_IFC_IMPORTER',
		'ASSIMP_BUILD_NO_IRR_IMPORTER',
		'ASSIMP_BUILD_NO_IRRMESH_IMPORTER',
		'ASSIMP_BUILD_NO_LWO_IMPORTER',
		'ASSIMP_BUILD_NO_LWS_IMPORTER',
		'ASSIMP_BUILD_NO_M3D_IMPORTER',
		'ASSIMP_BUILD_NO_MD2_IMPORTER',
		'ASSIMP_BUILD_NO_MD3_IMPORTER',
		'ASSIMP_BUILD_NO_MD5_IMPORTER',
		'ASSIMP_BUILD_NO_MDC_IMPORTER',
		'ASSIMP_BUILD_NO_MDL_IMPORTER',
		'ASSIMP_BUILD_NO_MMD_IMPORTER',
		'ASSIMP_BUILD_NO_MS3D_IMPORTER',
		'ASSIMP_BUILD_NO_NDO_IMPORTER',
		'ASSIMP_BUILD_NO_NFF_IMPORTER',
		-- 'ASSIMP_BUILD_NO_OBJ_IMPORTER',
		'ASSIMP_BUILD_NO_OFF_IMPORTER',
		'ASSIMP_BUILD_NO_OGRE_IMPORTER',
		'ASSIMP_BUILD_NO_OPENGEX_IMPORTER',
		'ASSIMP_BUILD_NO_PLY_IMPORTER',
		'ASSIMP_BUILD_NO_Q3BSP_IMPORTER',
		'ASSIMP_BUILD_NO_Q3D_IMPORTER',
		'ASSIMP_BUILD_NO_RAW_IMPORTER',
		'ASSIMP_BUILD_NO_SIB_IMPORTER',
		'ASSIMP_BUILD_NO_SMD_IMPORTER',
		'ASSIMP_BUILD_NO_STEP_IMPORTER',
		'ASSIMP_BUILD_NO_STL_IMPORTER',
		'ASSIMP_BUILD_NO_TERRAGEN_IMPORTER',
		'ASSIMP_BUILD_NO_X_IMPORTER',
		'ASSIMP_BUILD_NO_X3D_IMPORTER',
		'ASSIMP_BUILD_NO_XGL_IMPORTER',
		'ASSIMP_BUILD_NO_IQM_IMPORTER',
	
	-- Exporters
		
		'ASSIMP_BUILD_NO_COLLADA_EXPORTER',
		'ASSIMP_BUILD_NO_X_EXPORTER',
		'ASSIMP_BUILD_NO_STEP_EXPORTER',
		'ASSIMP_BUILD_NO_OBJ_EXPORTER',
		'ASSIMP_BUILD_NO_STL_EXPORTER',
		'ASSIMP_BUILD_NO_PLY_EXPORTER',
		'ASSIMP_BUILD_NO_3DS_EXPORTER',
		'ASSIMP_BUILD_NO_GLTF_EXPORTER',
		'ASSIMP_BUILD_NO_ASSBIN_EXPORTER',
		'ASSIMP_BUILD_NO_ASSXML_EXPORTER',
		'ASSIMP_BUILD_NO_X3D_EXPORTER',
		'ASSIMP_BUILD_NO_FBX_EXPORTER',
		'ASSIMP_BUILD_NO_PBRT_EXPORTER',
		'ASSIMP_BUILD_NO_M3D_EXPORTER',
		'ASSIMP_BUILD_NO_3MF_EXPORTER',
		'ASSIMP_BUILD_NO_ASSJSON_EXPORTER'
	}

	filter "system:windows"
        cppdialect "C++20"
		systemversion "latest"

	filter "configurations:Debug"
        optimize "Off"
        symbols "Full"

    filter "configurations:Release"
        optimize "Speed"
        symbols "FastLink"

    filter "configurations:Dist"
        optimize "Full"
        symbols "Off"