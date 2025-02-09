global-incdirs-y += include
global-incdirs-y += ./core/iwasm/include/ 
global-incdirs-y += ./core/app-framework/base/app 
srcs-y += wasi.c wasm.c wamr_ta.c

# Method 2 includes the static (trusted) library between the --start-group and
# --end-group arguments.
libnames += vmlib
libdirs += ./build
libdeps += ./build/libvmlib.a	
