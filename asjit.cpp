//
// Test author: Andreas Jonsson
//

#include "utils.h"
#include "armjit/as_jit_arm.h"
#include <fcntl.h>
#include <sys/mman.h> 
#include <sys/stat.h>

#define TESTNAME "asJITTest"
static const char *script;

extern "C"
{
int __aeabi_idiv(int a, int b)
{
    return a/b;
}
}
static void check (int test, const char * message, ...)
{
    if (test) {
        va_list args;
        va_start (args, message);
        vfprintf (stderr, message, args);
        va_end (args);
        fprintf (stderr, "\n");
        exit (EXIT_FAILURE);
    }
}

int main(int argc, char ** argv)
{
	struct stat s;
	
	printf("----------- HERE BE DRAGONS -----------\n");
	printf("%s : ", TESTNAME);
	int MINOR = ANGELSCRIPT_VERSION % 100;
	int MIDDLE = ANGELSCRIPT_VERSION / 100;
	int MAJOR = MIDDLE / 100;
	    MIDDLE %= 100;
	printf("AngelScript %d.%d.%d             \n", MAJOR, MIDDLE, MINOR);

 	asIScriptEngine *engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
	COutStream out;
	engine->SetMessageCallback(asMETHOD(COutStream,Callback), &out, asCALL_THISCALL);
    engine->SetEngineProperty(asEP_BUILD_WITHOUT_LINE_CUES, 1);
    engine->SetEngineProperty(asEP_INCLUDE_JIT_INSTRUCTIONS, 1);

    asIJITCompiler *jit = new asCJitArm(engine);
	if (argc == 1) {
		printf("Usage: asjit script.as JIT");
		exit(0);
	}
	int fd = open (argv[1], O_RDONLY); // open file for read
	check (fd < 0, "open %s failed: %s", argv[1], strerror (errno));
	/* Get the size of the file. */
    int status = fstat (fd, & s);
	check (status < 0, "stat %s failed: %s", argv[1], strerror (errno));
    int size = s.st_size;
	script = (char *) mmap (0, size, PROT_READ, MAP_PRIVATE, fd, 0);
	check (script == MAP_FAILED, "mmap %s failed: %s",
           argv[1], strerror (errno));
	//printf("%s\n", script);

    if (argc == 3) 
    {
		printf("Executing JIT version...");
        engine->SetJITCompiler(jit);
    } else {
		printf("Executing ByteCode version...");
	}
	printf("Building...");
	asIScriptModule *mod = engine->GetModule(0, asGM_ALWAYS_CREATE);
	mod->AddScriptSection(TESTNAME, script, strlen(script), 0);
	mod->Build();

	asIScriptContext *ctx = engine->CreateContext();
	ctx->Prepare(mod->GetFunctionByDecl("int TestInt(int a, int b, int c)"));
    ctx->SetArgDWord(0, 3);
    ctx->SetArgDWord(1, 5);
    ctx->SetArgDWord(2, 2);
	printf("Running...\n");
	double time = GetSystemTimer();

	int r = ctx->Execute();

	time = GetSystemTimer() - time;

	if( r != 0 )
	{
		printf("Execution didn't terminate with asEXECUTION_FINISHED\n");
		if( r == asEXECUTION_EXCEPTION )
		{
			printf("Script exception\n");
			asIScriptFunction *func = ctx->GetExceptionFunction();
			printf("Func: %s\n", func->GetName());
			printf("Line: %d\n", ctx->GetExceptionLineNumber());
			printf("Desc: %s\n", ctx->GetExceptionString());
		}
	}
	else
	{
		printf("Time = %f secs\n", time);
		printf("returned: %d\n", (int) ctx->GetReturnDWord());
	}


	ctx->Release();
	engine->Release();
    delete jit;
    return r;
}
