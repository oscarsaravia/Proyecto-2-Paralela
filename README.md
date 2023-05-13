// IMPLEMENTACION DE GUIDO
mpicxx -o implementacion1 implementacion1.cpp -lssl -lcrypto
mpirun -np 4 ./implementacion1 text.txt 12345678L

// IMPLEMENTACION DE OSCAR
mpicxx -o implementacion2 implementacion2.cpp -lssl -lcrypto
mpirun -np 4 ./implementacion2 text.txt 12345678L