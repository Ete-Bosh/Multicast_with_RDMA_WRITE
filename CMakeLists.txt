cmake_minimum_required(VERSION 3.10.3)

# Set RDMA-CORE
include_directories(~/rdma-core/build/include)
link_directories(~/rdma-core/build/lib)
#link_libraries(ibverbs)

# Target to build the client program
add_executable(receiver receiver.c)
add_executable(sender sender.c)
add_executable(gen_payload gen_payload.c)

target_link_libraries(receiver
    PRIVATE
      ibverbs
      m
	  rdmacm
)
target_link_libraries(sender
    PRIVATE
      ibverbs
      m
	  rdmacm
)
