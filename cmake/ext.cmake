
include("${CMAKE_CURRENT_LIST_DIR}/version.cmake")

CPMAddPackage(
        NAME argh
        GITHUB_REPOSITORY ${argh_GITHUB}
        GIT_TAG ${argh_TAG}
        DOWNLOAD_ONLY YES
        )

CPMAddPackage(
        NAME glfw
        GITHUB_REPOSITORY ${glfw_GITHUB}
        GIT_TAG ${glfw_TAG}
        OPTIONS
                "GLFW_INSTALL OFF"
                "GLFW_BUILD_DOCS OFF"
                "GLFW_BUILD_TESTS OFF"
                "GLFW_BUILD_EXAMPLES OFF"
        )

CPMAddPackage(
        NAME gli
        GITHUB_REPOSITORY ${gli_GITHUB}
        GIT_TAG ${gli_TAG}
        DOWNLOAD_ONLY YES
        )

CPMAddPackage(
        NAME glm
        GITHUB_REPOSITORY ${glm_GITHUB}
        GIT_TAG ${glm_TAG}
        DOWNLOAD_ONLY YES
        )

CPMAddPackage(
        NAME physfs
        GITHUB_REPOSITORY ${physfs_GITHUB}
        GIT_TAG ${physfs_TAG}
        OPTIONS
                "PHYSFS_ARCHIVE_ZIP ON"
                "PHYSFS_ARCHIVE_7Z OFF"
                "PHYSFS_ARCHIVE_GRP OFF"
                "PHYSFS_ARCHIVE_WAD OFF"
                "PHYSFS_ARCHIVE_HOG OFF"
                "PHYSFS_ARCHIVE_MVL OFF"
                "PHYSFS_ARCHIVE_QPAK OFF"
                "PHYSFS_ARCHIVE_SLB OFF"
                "PHYSFS_ARCHIVE_ISO9660 OFF"
                "PHYSFS_ARCHIVE_VDF OFF"
                "PHYSFS_BUILD_SHARED OFF"
                "PHYSFS_BUILD_TEST OFF"
        )

CPMAddPackage(
        NAME json
        GITHUB_REPOSITORY ${json_GITHUB}
        GIT_TAG ${json_TAG}
        DOWNLOAD_ONLY YES
        )

CPMAddPackage(
        NAME spdlog
        GITHUB_REPOSITORY ${spdlog_GITHUB}
        GIT_TAG ${spdlog_TAG}
        )

CPMAddPackage(
        NAME stb
        GITHUB_REPOSITORY ${stb_GITHUB}
        GIT_TAG ${stb_TAG}
        DOWNLOAD_ONLY YES
        )

CPMAddPackage(
        NAME tinyobjloader
        GITHUB_REPOSITORY ${tinyobjloader_GITHUB}
        GIT_TAG ${tinyobjloader_TAG}
        DOWNLOAD_ONLY YES
        )

CPMAddPackage(
        NAME Vulkan-Headers
        GITHUB_REPOSITORY ${Vulkan-Headers_GITHUB}
        GIT_TAG ${Vulkan-Headers_TAG}
        DOWNLOAD_ONLY YES
        )

CPMAddPackage(
        NAME Vulkan-Profiles
        GITHUB_REPOSITORY ${Vulkan-Profiles_GITHUB}
        GIT_TAG ${Vulkan-Profiles_TAG}
        DOWNLOAD_ONLY YES
        )

CPMAddPackage(
        NAME volk
        GITHUB_REPOSITORY ${volk_GITHUB}
        GIT_TAG ${volk_TAG}
        DOWNLOAD_ONLY YES
        )

CPMAddPackage(
        NAME VulkanMemoryAllocator
        GITHUB_REPOSITORY ${VulkanMemoryAllocator_GITHUB}
        GIT_TAG ${VulkanMemoryAllocator_TAG}
        DOWNLOAD_ONLY YES
        )

CPMAddPackage(
        NAME IconFontCppHeaders
        GITHUB_REPOSITORY ${IconFontCppHeaders_GITHUB}
        GIT_TAG ${IconFontCppHeaders_TAG}
        DOWNLOAD_ONLY YES
        )

if(IMGUI_DOCKING)
        CPMAddPackage(
                NAME imgui
                GITHUB_REPOSITORY ${imgui_docking_GITHUB}
                GIT_TAG ${imgui_docking_TAG}
                DOWNLOAD_ONLY YES
                )
else()
        CPMAddPackage(
                NAME imgui
                GITHUB_REPOSITORY ${imgui_GITHUB}
                GIT_TAG ${imgui_TAG}
                DOWNLOAD_ONLY YES
                )
endif()

CPMAddPackage(
        NAME SPIRV-Headers
        GITHUB_REPOSITORY ${SPIRV-Headers_GITHUB}
        GIT_TAG ${SPIRV-Headers_TAG}
        DOWNLOAD_ONLY YES
        )

CPMAddPackage(
        NAME SPIRV-Tools
        GITHUB_REPOSITORY ${SPIRV-Tools_GITHUB}
        GIT_TAG ${SPIRV-Tools_TAG}
        OPTIONS
                #"SKIP_SPIRV_TOOLS_INSTALL ON"
                "SPIRV_SKIP_TESTS ON"
                "SPIRV_SKIP_EXECUTABLES ON"
        )

CPMAddPackage(
        NAME glslang
        GITHUB_REPOSITORY ${glslang_GITHUB}
        GIT_TAG ${glslang_TAG}
        OPTIONS
                "ENABLE_CTEST OFF"
                "ENABLE_GLSLANG_BINARIES OFF"
                "BUILD_EXTERNAL OFF"
                #"SKIP_GLSLANG_INSTALL ON"
        )

CPMAddPackage(
        NAME shaderc
        GITHUB_REPOSITORY ${shaderc_GITHUB}
        GIT_TAG ${shaderc_TAG}
        OPTIONS
                "SHADERC_SKIP_TESTS ON"
                "SHADERC_SKIP_EXAMPLES ON"
                #"SHADERC_SKIP_INSTALL ON"
                "SHADERC_SKIP_COPYRIGHT_CHECK ON"
        )

CPMAddPackage(
        NAME PicoSHA2
        GITHUB_REPOSITORY ${PicoSHA2_GITHUB}
        GIT_TAG ${PicoSHA2_TAG}
        DOWNLOAD_ONLY YES
        )

if(LIBLAVA_TEST)
        CPMAddPackage(
                NAME Catch2
                GITHUB_REPOSITORY ${Catch2_GITHUB}
                GIT_TAG ${Catch2_TAG}
                )
endif()
