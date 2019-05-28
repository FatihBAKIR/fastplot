from conans import ConanFile, CMake, tools

class FastPlotConanfile(ConanFile):
    name = "fastplot"
    version = "0.1"
    license = "<Put the package license here>"
    author = "<Put your name here> <And your email here>"
    url = "<Package recipe repository url here, for issues about the package>"
    description = "<Description of Xbeeserver here>"
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake_paths"
    export_sources = "*"

    default_options = {
        "boost:without_context" : True,
        "boost:without_coroutine" : True,
        "boost:without_fiber" : True,
        "boost:without_locale" : True,
        "boost:without_test" : True,
        "boost:without_graph_parallel" : True,
        "boost:without_serialization" : True,
        "glad:profile": "core",
        "glad:api_type": "gl",
        "glad:api_version": "3.1"
    }

    def requirements(self):
        self.requires("glm/0.9.9.4@g-truc/stable")
        self.requires("glad/0.1.29@bincrafters/stable")
        self.requires("gsl_microsoft/20180102@bincrafters/stable")
        if (self.settings.os != "Emscripten"):
            self.requires("glfw/3.3@bincrafters/stable")

    def source(self):
        pass

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        self.copy("*", dst="bin", src="bin")
