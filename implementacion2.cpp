// bruteforceNaive.c
// Tambien cifra un texto cualquiera con un key arbitrario.
// OJO: asegurarse que la palabra a buscar sea lo suficientemente grande
//   evitando falsas soluciones ya que sera muy improbable que tal palabra suceda de
//   forma pseudoaleatoria en el descifrado.
//>> mpicc bruteforce.c -o desBrute
//>> mpirun -np <N> desBrute

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <unistd.h>
#include <openssl/des.h>
#include <openssl/err.h>

#include <iostream>
#include <string>

// descifra un texto dado una llave
void decrypt(long key, char *ciph, int len)
{
  DES_cblock deskey;
  DES_key_schedule keysched;
  memset(deskey, 0, 8);
  memcpy(deskey, &key, sizeof(long));
  DES_set_key(&deskey, &keysched);
  DES_ecb_encrypt((unsigned char(*)[8])ciph, (unsigned char(*)[8])ciph, &keysched, DES_DECRYPT);
}

// cifra un texto dado una llave
void encrypt(long key, char *ciph)
{
  DES_cblock deskey;
  DES_key_schedule keysched;
  memset(deskey, 0, 8);
  memcpy(deskey, &key, sizeof(long));
  DES_set_key(&deskey, &keysched);
  DES_ecb_encrypt((unsigned char(*)[8])ciph, (unsigned char(*)[8])ciph, &keysched, DES_ENCRYPT);
}

// palabra clave a buscar en texto descifrado para determinar si se rompio el codigo
char search[] = "Esta es una prueba de proyecto 2";
int tryKey(long key, char *ciph, int ciphlen)
{
  char temp[ciphlen + 1]; // Add space for null terminator
  memcpy(temp, ciph, ciphlen);
  decrypt(key, temp, ciphlen);
  temp[ciphlen] = '\0'; // Add null terminator
  return strstr(temp, search) != NULL;
}

// long the_key = 123456L;
// 2^56 / 4 es exactamente 18014398509481983
// long the_key = 18014398509481983L;
// long the_key = 18014398509481983L +1L;

int main(int argc, char *argv[])
{ // char **argv

  int N, id;
  long upper = (1L << 56); // upper bound DES keys 2^56
  long mylower, myupper;
  MPI_Status st;
  MPI_Request req;

  if (argc != 3)
  {
    printf("Usage: %s filename\n", argv[0]);
    return 1;
  }

  char *filename = argv[1];
  // long the_key = strtol(argv[2], NULL, 10);

  std::string arg = argv[2];
  long int the_key = std::stol(arg);

  // Open the file for reading
  FILE *file = fopen(filename, "r");
  if (file == NULL)
  {
    printf("Error opening file %s\n", filename);
    return 1;
  }

  // Get the file size
  fseek(file, 0, SEEK_END);
  long fsize = ftell(file);
  fseek(file, 0, SEEK_SET);

  // Read the file contents into memory
  char *filedata = (char *)malloc(fsize + 1);
  fread(filedata, fsize, 1, file);
  fclose(file);

  // Pad the file data to a multiple of 8 bytes
  int padlen = 8 - (fsize % 8);
  char data[fsize + padlen + 1];
  memset(data, 0, fsize + padlen + 1);
  memcpy(data, filedata, fsize);
  free(filedata);

  printf("LECTURA!!!%s \n", data);
  // char eltexto[] = "Esta es una prueba de proyecto 2";
  int ciphlen = strlen(data);

  // Pad the cipher buffer to a multiple of 8 bytes
  char cipher[ciphlen + padlen + 1];
  memset(cipher, 0, ciphlen + padlen + 1);
  memcpy(cipher, data, ciphlen);
  encrypt(the_key, cipher);

  MPI_Comm comm = MPI_COMM_WORLD;

  // INIT MPI
  MPI_Init(NULL, NULL);
  double start = MPI_Wtime ();
  MPI_Comm_size(comm, &N);
  MPI_Comm_rank(comm, &id);

  long found = 0L;
  int ready = 0;

  // distribuir trabajo de forma naive

  // non blocking receive, revisar en el for si alguien ya encontro
  MPI_Irecv(&found, 1, MPI_LONG, MPI_ANY_SOURCE, MPI_ANY_TAG, comm, &req);
  int blockSize = 1000;
  for (long long i = id * blockSize; i < upper; i += (N*blockSize))
  {
    for (long long j = i ; j < blockSize * N + i; j += 1){
        MPI_Test(&req, &ready, MPI_STATUS_IGNORE);
        if (ready)
        break; // ya encontraron, salir
        // printf("j: %d and id %d\n", j, id);
        if (tryKey(j, cipher, ciphlen))
        {
        found = j;
        printf("Process %d found the key KEY: %d\n", id, found);
        for (int node = 0; node < N; node++)
        {
            MPI_Send(&found, 1, MPI_LONG, node, 0, comm); // avisar a otros
        }
        break;
        }
    }
    if (found != 0){
        break;
    }
  }

  // wait y luego imprimir el texto
  if (id == 0)
  {
    MPI_Wait(&req, &st);
    decrypt(found, cipher, ciphlen);
    printf ("Found using %i nodes in %lf sec!\n", N, MPI_Wtime()- start);
    printf("Key = %li\n\n", found);
    printf("%s\n", cipher);
  }
  printf("Process %d exiting\n", id);

  // FIN entorno MPI
  MPI_Finalize();
}
