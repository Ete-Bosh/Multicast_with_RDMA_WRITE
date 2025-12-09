
A codebase for a project - RDMA multicast with WRITE.

### gen_payload.c

It's used for generating arbitrary size of payload. You can run it with

```
./gen_payload  <size> <output_filename>
```

### sender.c

It's used for multicasting payload to a pre-defined multicast group. You can run it with

```
./sender [-p port][-b chunk size][-f payload file][-r receiver list][-s send rate]
```

### receiver.c

It's used for receiving packets

## How to run

Make sure you already have RDMA links and library to compile C files. If your host doesn't have RNIC like Mellanox series, you can resort to [this project](https://github.com/Ete-Bosh/RDMA-toy) to install soft-roce. If you have any question, please send me an email to jwan20@cs.jhu.edu and I am glad to help you.

1. Join multicast group:

   ```
   sudo ip route add 239.255.0.1/32 dev ens192
   sudo smcroutectl join ens192 239.255.0.1
   netstat -g | grep 239.255.0.1
   ```

   239.255.0.1 is the multicast address, ens192 is your device name

2. Compile `sender.c` and `receiver.c`

   Please make sure you have put the right path of rdma library in CMakeLists.txt. Then run build.sh

3. Compile `gen_payload.c` and generate your payload.

4. Run sender and receiver

---

For DMA part, you need to install [DOCA framework](https://developer.nvidia.com/networking/doca). Please check [here](https://docs.nvidia.com/doca/sdk/doca-installation-guide-for-linux/index.html) for more instructions. 

1. Install DOCA-Host on your host

2. Install BF-Bundle on your DPU

3. Since I don't have Internet access on DPU, you probably need to download meson and ninja deb on your host, copy them to the DPU and install them.

4. Build DMA Copy application:

   ```
   cd /opt/mellanox/doca/applications/
   meson /tmp/build -Denable_all_applications=false -Denable_dma_copy=true
   ninja -C /tmp/build
   ```

5. Check your dev info 

   ```
   /opt/mellanox/doca/tools/doca_caps --list-devs
   ```

   and choose DOCA Comm Channel device PCI address and DOCA Comm Channel device representor PCI address. See [here](https://docs.nvidia.com/doca/sdk/doca-dma-copy-application-guide/index.html) for more information.

6. Run `./doca_dma_copy` on DPU

   ```
   Usage: doca_dma_copy [DOCA Flags] [Program Flags]
    
   DOCA Flags:
     -h, --help                        Print a help synopsis
     -v, --version                     Print program version information
     -l, --log-level                   Set the (numeric) log level for the program <10=DISABLE, 20=CRITICAL, 30=ERROR, 40=WARNING, 50=INFO, 60=DEBUG, 70=TRACE>
     --sdk-log-level                   Set the SDK (numeric) log level for the program <10=DISABLE, 20=CRITICAL, 30=ERROR, 40=WARNING, 50=INFO, 60=DEBUG, 70=TRACE>
     -j, --json <path>                 Parse command line flags from an input json file
    
   Program Flags:
     -f, --file                        Full path to file to be copied/created after a successful DMA copy
     -p, --pci-addr                    DOCA Comm Channel device PCI address
     -r, --rep-pci                     DOCA Comm Channel device representor PCI address (needed only on DPU)
   ```

7. Run `./doca_dma_copy` on host

   ```
   ./doca_dma_copy -p 3b:00.0 -f send.txt
   ```

---
   
   For congestion control part, you can find codes of DCQCN algorithm and an implementation of event scheduler in CC folder. Besides, I also include a google test file to verify the correctness of DCQCN. It describes all potential behavior all DCQCN events.  You can use CMake file in the folder to compile and run the test directly. It should shows all tests passed.
