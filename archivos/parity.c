
/*
 Name       : parity.c
 Autor      : Francisco Javier Moya Viedma
 */

#include <stdio.h>	// para printf()
#include <stdlib.h>	// para exit()
#include <sys/time.h>	// para gettimeofday(), struct timeval

#define WSIZE 8*sizeof(int)
#define TEST		1
#define COPY_PASTE_CALC	0

#if ! TEST	
	#define NBITS	20
	#define SIZE (1<<NBITS)
	unsigned lista[SIZE];
	#define RESULT (1<<NBITS-1)
#else

// Quitar "espacio en blanco" que aparece en "-----* /" para poder utilizar unos datos concretos...
 
/*----------------------------------------------------------------------------------------------*/
	#define SIZE 4
	unsigned lista[SIZE]={0x80000000, 0x00100000, 0x00000800, 0x00000001};
	#define RESULT 4
/*----------------------------------------------------------------------------------------------* /
	#define SIZE 8
	unsigned lista[SIZE]={0x7fffffff, 0xffefffff, 0xfffff7ff, 0xfffffffe,
			      0x01000024, 0x00356700, 0x8900ac00, 0x00bd00ef}; 
	#define RESULT 8

/*----------------------------------------------------------------------------------------------* /
	#define SIZE 8
	unsigned lista[SIZE]={0x0, 0x10204080, 0x3590ac06, 0x70b0d0e0,
			      0xffffffff, 0x12345678, 0x9abcdef0, 0xcafebeef};
	#define RESULT 2

/*----------------------------------------------------------------------------------------------*/

#endif 

int resultado = 0;

int parity1(unsigned* array, int len)
{

int i, j, val, result = 0;
unsigned x;

        for (j = 0; j < len; j++){
		x = array[j];
		val =  0;		// val libre en cada 
                for (i = 0; i < WSIZE; i++) {
                        val ^= x & 1 ;	// acumular XOR	lateralmente
			x >>= 1 ; // desplazar 
                        
	        }        
	result += val;			// acumular suma paridades
	}
        return result;

}

int parity2(unsigned* array, int len)
{
	int i, val, result = 0;
	unsigned x;

        for (i = 0; i < len; i++){
		x = array[i];
		val =  0;		// val libre en cada 
                do{
                        val ^= x & 1 ;	// acumular XOR	lateralmente
			x >>= 1 ;
                        
	        }  while ( x );      
	result += val;			// acumular suma paridades
	}
        return result;

}

int parity3(unsigned* array, int len)
{

	int val;
	int i;
	unsigned x;
	int result = 0;
	for (i = 0; i < len; i++) {
		x = array[i];
		val=0;
		while (x) {
			val ^= x;
			x >>= 1;
		}
		result += val & 0x1;
	}
	return result;

	
}

/*
 * Versión C de CS:APP
 */
int parity4(unsigned* array, int len) {

        
	int val;
	int i;
	unsigned x;
	int result = 0;

	for (i = 0; i < len; i++) {
		x = array[i];
		val = 0;
		asm(
				"ini3:						\n\t"
				"xor %[x], %[v]			\n\t"
				"shr $1, %[x]			\n\t"
				"test %[x], %[x]       		 \n\t"
				"jnz ini3				\n\t"
				: [v]"+r"(val) // e/s: inicialemnte 0, salida valor final
				: [x]"r"(x)// entrada: valor del elemento
		);
		result += val & 0x1;
	}
	return result;
}

int parity5(unsigned* array, int len) {

     int  i, j;
    unsigned x;
    int val, result=0;

  	for(i=0;i<len;i++){
   		x = array[i];
		// La idea es someter al elemento del array a XOR y desplazamientos sucesivos cada vez a mitad de distancia ( 16, 8, 4, 2, 1 ).
		for(j=16;j>=1;j/=2){
			x^=x>>j;
		}
		result += x & 0x1;
   	}
	
return result;

}
int parity6(unsigned* array, int len) {

        int j;
	unsigned entero = 0;


	int resultado = 0;

	for (j = 0; j < len; j++) { //Cuando acabe de recorrer el vector se saldrá del bucle

		entero = array[j]; //Cargo en entero el siguiente numero de la lista
		asm(
				"mov	%[x], 	%%edx		\n\t"
				"shr	$16,	%%edx		\n\t"
				"xor	%[x],	%%edx		\n\t"
				"xor	%%dh,	%%dl		\n\t"
				"setpo  %%dl				\n\t"
				"movzx	%%dl,	%[x]		\n\t"
				: [x] "+r" (entero) // input
				:
				: "edx"//Clobber
		);
		resultado += entero;
	}
	return resultado;

}

void crono(int (*func)(), char* msg){
    struct timeval tv1,tv2;	// gettimeofday() secs-usecs
    long           tv_usecs;	// y sus cuentas

    gettimeofday(&tv1,NULL);
    resultado = func(lista, SIZE);
    gettimeofday(&tv2,NULL);

    tv_usecs=(tv2.tv_sec -tv1.tv_sec )*1E6+
             (tv2.tv_usec-tv1.tv_usec);

#if COPY_PASTE_CALC	// cuando hagamos c-p a Calc
	printf(	"%ld" "\n", tv_usecs); 
#else
    printf("resultado = %d\t", resultado);
    printf("%s:%9ld us\n", msg, tv_usecs);
#endif

}

int main()
{
#if ! TEST // inicializar array
    int i;			// inicializar array
    for (i=0; i<SIZE; i++)	// se queda en cache
	 lista[i]=i;
#endif // solo para ejemplo grande

	crono(parity1, "parity1 (lenguaje C -	for)     "); // cambiar nombres
        crono(parity2, "parity2 (lenguaje C -	while)   "); // y descripción
      	crono(parity3, "parity3 (    Ahorrando máscara  )");
        crono(parity4, "parity4 (Sumando bytes completos)");
        crono(parity5, "parity5 (        SSSE3          )");
        crono(parity6, "parity6 (        SSSE4.2        )"); 
	// añadiremos más versiones

#if ! 	COPY_PASTE_CALC
	printf("calculado = %d\n", RESULT);
#endif


    exit(0);
}
