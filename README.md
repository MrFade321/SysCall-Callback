# NtSetInformationProcess PoC using ProcessInstrumentationCallback

This repository demonstrates a Proof of Concept (PoC) utilizing `NtSetInformationProcess()` with the `ProcessInstrumentationCallback` (ID 40) to set up a callback for all system calls within processes. The PoC illustrates how this method can be achieved by injecting a DLL into a target process.

You can register the call back to a remote proces (externally) however it will require a handle with `PROCESS_SET_INFORMATION` and `SeDebugPrivilege` access to the target process.

## Usage

To use this PoC:

1. Simply Inject the DLL into the target process.

## Detection

Detecting if your process is affected by such a "hook" can be done in the following ways:

- Check if `InstrumentationCallback` is not null.
-  if your process is using the Callback, Verify if the callback pointer is located within your own modules in memory.
  
If these checks fail, it may indicate that an external entity has registered its own instrumentation callback within your process.

---
(This is by no means a new method)
Lots of credit to :
https://splintercod3.blogspot.com/p/weaponizing-mapping-injection-with.html

for a great write up and demonstrations. 
