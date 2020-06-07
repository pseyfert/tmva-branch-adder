from ROOT import gInterpreter


def hack_compilation():
    from ROOT import gSystem
    cmd = gSystem.GetMakeSharedLib()
    import subprocess
    # modern way for python3 to avoid byte vs. string mess
    # fallback to python2
    try:
        compiler = subprocess.getoutput("root-config --cxx")
    except AttributeError:
        compiler = subprocess.check_output(["root-config", "--cxx"])
        compiler = compiler[:-1]  # remove trailing line feed
    # FIXME: no idea what this if statement was meant to do
    if compiler.find(" "):
        print("WILL NEED TO FIX BUILD COMMAND")
        print("old command:")
        print(cmd)
        # the GetMakeSharedLib is broken on one of my systems because the
        # compiler command contains a white space "ccache g++"
        # gInterpreter then uses only the first word "ccache"
        cmd = cmd.replace("ccache", compiler)
        gSystem.SetMakeSharedLib(cmd)
        print("new command:")
        print(cmd)


hack_compilation()
gInterpreter.LoadMacro("../src/forbidden_branchnames.cpp+")
gInterpreter.LoadMacro("../src/reader_wrapper.cpp+")
from ROOT import reader_wrapper
