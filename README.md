# Multi-Threaded-Bakery-Simulation
This project simulates a bakery using multi-threading and semaphores in C. A supplier delivers ingredient pairs from a file to multiple chefs, who wait for specific ingredients to bake cakes. Demonstrates thread synchronization, semaphore usage, and concurrent programming.



### General Purpose of the Program

This program is a project application that involves system programming and the use of multi-threading. The main purpose of the program is to coordinate between pastry chefs making cakes using various materials and a wholesaler providing these materials.

### General Workflow of the Program

1. **Material Supply:**
   - The program includes a wholesaler thread that reads pairs of materials from a file. The materials are read sequentially from the file and each pair is distributed to the relevant chef threads.

   1.1 **Reading format:** 
   ```
   UK
   YU
   .
   .
   .
   ```

2. **Work of the Chefs:**
   - Each of the six chef threads requires a specific pair of materials, and when these materials are provided, they prepare their cakes.

3. **Threads and Synchronization:**
   - After providing the materials, the wholesaler thread wakes up the chefs according to the appropriate material pairs.
   - Each chef thread starts working when the materials are provided, prepares the cake, and then delivers it to the wholesaler.
   - Semaphores are used to ensure synchronization during the material supply and cake delivery processes.

### Core Components and Functions of the Program

1. **Data Structures:**
   - The "Materials" structure holds two material names (STRUCT).
   - The "chef_requirements" array determines the pairs of materials each chef needs (the wholesaler will direct the materials to the correct chef according to this during runtime).

2. **Semaphores:**
   - "material_semaphore": Controls access to the material depot.
   - "chef_semaphore": Separate semaphores for each chef, waking them up when the appropriate materials are provided.
   - "delivery_semaphore": Ensures the wholesaler waits for the cake to be delivered.

3. **Functions:**

   3.1 **Wholesaler Thread Function:**
   - Reads pairs of materials from the file and writes the appropriate material names to the "material_depot" (struct).
   - After performing the necessary combination check, delivers the materials and wakes up the relevant chefs.
   - Once all materials are provided, informs the chefs that the work is done via a globally maintained variable.

   3.2 **Chef Thread Function:**
   - Waits for the required materials.
   - Once the materials are provided, prepares the cake and delivers it to the wholesaler.

   3.3 **Main Function:**
   - Takes the name of the material file from the command line (argument line).
   - Initializes semaphores.
   - Creates and starts the chef and wholesaler threads.
   - Waits for the wholesaler to finish its work, then terminates all threads.
   - Cleans up the semaphores before the program terminates.

### Program Execution Flow

1. When the program starts, it takes the name of the material file from the command line.
2. Semaphores and threads (wholesaler and chefs) are created.
3. The wholesaler thread starts reading materials from the file and passes the material pairs to the chefs.
4. Chefs prepare their cakes and deliver them to the wholesaler once the required materials are provided.
5. Once all materials are provided, the wholesaler informs the chefs that the work is done.
6. All threads are terminated, and the program shuts down gracefully.

### Steps to Run the Program

1. If the code is not compiled, this process should be done via the terminal.
   - Command: `gcc 170421929_pasta.c -o program`
   
   **A compiled version named "program" is also included in the submitted assignment files.**

2. If the code is already compiled, the file should be run via the terminal.
   - Command: `./program -i file_path`
   
   **Since the code takes material information from a "txt" file, you can use the provided "malzemeler.txt" file with the following command:**
   - Command: `./program -i malzemeler.txt`

### System Evaluation

1. The system check with VALGRIND shows the following output:
   ```
   valgrind ./program -i malzemeler.txt
   ==4823== Memcheck, a memory error detector
   ==4823== Copyright (C) 2002-2022, and GNU GPL'd, by Julian Seward et al.
   ==4823== Using Valgrind-3.19.0 and LibVEX; rerun with -h for copyright info
   ==4823== Command: ./program -i malzemeler.txt
   ==4823==
   .
   .<Code output>
   .
   ==4823==
   ==4823== HEAP SUMMARY:
   ==4823==     in use at exit: 0 bytes in 0 blocks
   ==4823==   total heap usage: 16 allocs, 16 frees, 7,520 bytes allocated
   ==4823==
   ==4823== All heap blocks were freed -- no leaks are possible
   ==4823==
   ==4823== For lists of detected and suppressed errors, rerun with: -s
   ==4823== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
   ```

2. The zombie process check using "ps" command:
   ```
   Terminal Input: ps -aux

   USER         PID %CPU %MEM    VSZ   RSS TTY      STAT START   TIME COMMAND
   runner         1  0.8  0.0 2664056 48028 ?       Ssl  12:31   2:07 /nix/store/r1wdmv4g3b
   runner        15  0.5  0.3 11641480 232508 ?     Sl   12:33   1:27 pid2 --enable-source-
   runner        28  0.0  0.0   8404  4864 pts/0    Ss+  12:33   0:00 /nix/store/5q96kfjc8b
   runner        46  0.0  0.0   3804  2560 ?        S    12:33   0:00 nix-editor --return-o
   runner        69  0.1  0.1 1147124 88444 ?       Sl   12:33   0:15 /nix/store/l2nfqy8hhq
   runner        70  0.0  0.0 1245948 25616 ?       Sl   12:33   0:00 /nix/store/ls2pzi5wn9
   runner      4966  0.0  0.0   8388  4864 pts/1    Ss   16:25   0:00 /nix/store/5q96kfjc8b
   runner      5725  0.0  0.0   8920  3200 pts/1    R+   16:37   0:00 ps -aux
   ```

   **After running the program, examining the current processes in the system shows that no processes have a "z" character in the "STAT" (status) class. This indicates that there are no "zombie" processes in the system.**
