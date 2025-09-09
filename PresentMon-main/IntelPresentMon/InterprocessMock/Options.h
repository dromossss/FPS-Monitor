#pragma once
#include "../CommonUtilities/cli/CliFramework.h"
#include <string>
#include <vector>

namespace pmon::ipc::mock::opt
{
	using namespace pmon::util;
	using namespace pmon::util::cli;
	struct Options : public OptionsBase<Options>
	{
		// add options and switches here to augment the CLI
		Flag testF{ this, "--test-f", "Output test string from test function f() in ipc lib" };
		Flag basicMessage{ this, "--basic-message", "Do minimal IPC test transferring string across shared memory" };
		Flag destroyUptr{ this, "--destroy-uptr", "Test which destroys the uptr that is constructed by default" };
		Flag makeDestroyUptr{ this, "--make-destroy-uptr", "Test which makes and destroys the uptr such that leaks can be detected" };
		Flag sharedRootBasic{ this, "--shared-root-basic", "Basic test of creating root in shared memory" };
		Flag sharedRootRetained{ this, "--shared-root-retained", "Test retaining root in shared memory until ack" };
		Flag clientFree{ this, "--client-free", "Allocated in shm in server and free from client" };
		Flag deep{ this, "--deep", "Testing sequence for deeply nested structure with multiple container layers" };
		Flag cloneHeap{ this, "--clone-heap-to-shm", "Create simple root on heap, clone to shm" };
		Flag cloneHeapDeep{ this, "--clone-heap-deep-to-shm", "Create deep root on heap, clone to shm" };
		Flag cloneHeapDeep2{ this, "--clone-heap-deep-to-shm-2", "Create deep root on heap, clone to shm (vector of <obj> instead of <uptr>)" };
		Flag basicIntro{ this, "--basic-intro", "Create introspection root so middleware code can access it" };
		Option<std::string> introNsm{ this, "--intro-nsm", "", "Name to use when creating NSM for introspection data" };

		static constexpr const char* description = "Application to mock interprocess communication endpoints for testing";
		static constexpr const char* name = "InterprocessMock.exe";
	private:
		MutualExclusion ex_{ testF, basicMessage, destroyUptr, makeDestroyUptr, sharedRootBasic,
			sharedRootRetained, clientFree, deep, cloneHeap, cloneHeapDeep, cloneHeapDeep2, basicIntro };
	};
}