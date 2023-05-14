// BRUTE FORCE NAIVE OSCAR P.
mpicc -I/direccion/a/carpeta/include/de/openssl -L/direccion/a/carpeta/lib/de/openssl bruteForceNaive.cpp -o bruteForceNaive -lssl -lcrypto -lstdc++
mpirun -np 4 bruteForceNaive input.txt 123456789L

// IMPLEMENTACION DE GUIDO
mpicc -I/direccion/a/carpeta/include/de/openssl -L/direccion/a/carpeta/lib/de/openssl implementacion1.cpp -o implementacion1 -lssl -lcrypto -lstdc++
mpirun -np 4 implementacion1 input.txt 123456789L

// IMPLEMENTACION DE OSCAR S.
mpicc -I/direccion/a/carpeta/include/de/openssl -L/direccion/a/carpeta/lib/de/openssl implementacion2.cpp -o implementacion2 -lssl -lcrypto -lstdc++
mpirun -np 4 implementacion2 input.txt 123456789L
