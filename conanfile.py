from conans import ConanFile, CMake, tools

class SimpleGrammarCppConan(ConanFile):
    name     = "simple-application-cpp"
    version  = "1.0.0"
    author   = "Maxime Philbert"
    homepage = ""
    url      = ""
    settings = "os", "compiler", "build_type", "arch"
    requires = [
        "vsdk-csdk/[>=1.0.8 <2.0]@vivoka/customer"
    ]
    default_options = {
        "*:shared": True
    }

    generators = "cmake", "virtualrunenv"
    exports_sources = "src/*"

    def config_options(self):
        if self.settings.os == "Windows":
            self.options["vsdk-csdk"].shared = False

    def system_requirements(self):
        packages = []
        if tools.os_info.is_linux:
            if tools.os_info.linux_distro == "fedora" or tools.os_info.linux_distro == "centos":
                packages.append("portaudio")
                packages.append("portaudio-devel")
            elif tools.os_info.linux_distro == "debian" or tools.os_info.linux_distro == "ubuntu":
                packages.append("libportaudio2")
                packages.append("portaudio19-dev")
            elif tools.os_info.linux_distro == "manjaro":
                packages.append("portaudio")

        if len(packages) != 0:
            installer = tools.SystemPackageTool()
            for package in packages:
                installer.install(package)

    def requirements(self):
         if self.settings.os == "Windows":
             self.requires("portaudio/v190600.20161030@vivoka/customer")

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        self.copy(self.name, dst="bin")

    def imports(self):
        self.copy("*.dll", dst="bin", keep_path=False)
