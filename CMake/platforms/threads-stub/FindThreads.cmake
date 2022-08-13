# Stub out the Threads package.
# Some platforms do not have a system threads library but SDL threads are supported.
add_library(Threads::Threads INTERFACE IMPORTED GLOBAL)
