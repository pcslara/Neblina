/*
 * main.c
 *
 *  Created on: Mar 4, 2015
 *      Author: Lu’s Tarrataca
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>

typedef struct Complex_t
{
	double realPart;
	double imaginaryPart;

} Complex;


int countNumberOfDigits( int number ){

	int numberOfDigits=0;

	while( number != 0 ){
		number=number/10;
		numberOfDigits++;
	}

	return numberOfDigits;

}

Complex newComplexNumber( double realPart, double imaginaryPart ){

	Complex result;

	result.realPart = realPart;
	result.imaginaryPart = imaginaryPart;

	return result;


}

double complexReal( Complex number ){ return number.realPart; }
double complexImag( Complex number ){ return number.imaginaryPart; }

Complex complexConj( Complex number){

	Complex result;

	result.realPart = number.realPart;
	result.imaginaryPart = (-1) * number.imaginaryPart;

	return result;

}

Complex complexAdd( Complex number1, Complex number2 ){

	Complex result;

	result.realPart = number1.realPart + number2.realPart;
	result.imaginaryPart = number1.imaginaryPart + number2.imaginaryPart;

	return result;


}

Complex complexSubtract( Complex number1, Complex number2 ){

	Complex result;

	result.realPart = number1.realPart - number2.realPart;
	result.imaginaryPart = number1.imaginaryPart - number2.imaginaryPart;

	return result;


}

Complex complexMultiply( Complex number1, Complex number2 ){

	Complex result;

	result.realPart = number1.realPart*number2.realPart - number1.imaginaryPart*number2.imaginaryPart;
	result.imaginaryPart = number1.imaginaryPart*number2.realPart + number1.realPart*number2.imaginaryPart;

	return result;

}

Complex complexDivide( Complex number1, Complex number2 ){

	Complex result;

	result.realPart = (number1.realPart*number2.realPart + number1.imaginaryPart*number2.imaginaryPart) / ( number2.realPart * number2.realPart + number2.imaginaryPart * number2.imaginaryPart );
	result.imaginaryPart = (number1.imaginaryPart*number2.realPart - number1.realPart*number2.imaginaryPart) / ( number2.realPart * number2.realPart + number2.imaginaryPart * number2.imaginaryPart );

	return result;

}

char* complexNumberToString( Complex number, int precision ){

	int numberOfDigits = countNumberOfDigits( precision );

	char precisionString[ numberOfDigits + 1 ];

	sprintf( precisionString, "%d", precision );

	char* formattedString = (char*) malloc( sizeof( char ) * ( 12 ) );
	char* result = (char*) malloc( sizeof( char ) * ( precision * 3 ) );

	strcat( formattedString, "%." );
	strcat( formattedString, precisionString );
	strcat( formattedString, "f%+." );
	strcat( formattedString, precisionString );
	strcat( formattedString, "fi\t" );

	sprintf( result, formattedString, complexReal( number ), complexImag( number ) );

	return result;


}

Complex** buildR_X( double theta ){

	Complex** R_X = (Complex**) malloc( sizeof( Complex* ) * 2);

	R_X[ 0 ] = (Complex*) malloc( sizeof( Complex ) * 2);
	R_X[ 1 ] = (Complex*) malloc( sizeof( Complex ) * 2);

	Complex value1 = newComplexNumber( cos( theta / 2 ), 0 );
	Complex value2 = newComplexNumber( 0, -sin( theta / 2) );

	R_X[ 0 ][ 0 ] = value1;
	R_X[ 0 ][ 1 ] = value2;
	R_X[ 1 ][ 0 ] = value2;
	R_X[ 1 ][ 1 ] = value1;

	return R_X;
}

void freeMemory( Complex** R_X ){

	free( R_X[0] );
	free( R_X[1] );
	free( R_X );

}

Complex calculateIndividualIndexIteratively( int row, int column, int dimension, Complex** R_X ){

	int dimensionToThePowerOf2 = dimension * dimension;

	if( row > dimensionToThePowerOf2 || column > dimensionToThePowerOf2 ){

		printf("ERROR: invalid row or column dimension");

	}

	if( dimension == 2 ){

		return R_X[ row ][ column ];

	}

	int totalNumberOfIterations = (int) log2( dimension );

	int subMatricesDimension = 0;

	int rRow 	= 0;
	int rColumn = 0;

	int rPrimeColumn = 0;
	int rPrimeRow	 = 0;

	int rowAux = row;
	int columnAux = column;
	int dimensionAux = dimension;

	Complex result = newComplexNumber( 1, 0 );

	int iterationCounter = 0;

	for( iterationCounter = 0; iterationCounter < totalNumberOfIterations; iterationCounter++ ){

		subMatricesDimension = dimensionAux / 2;

		rRow 	= rowAux /subMatricesDimension;
		rColumn = columnAux /subMatricesDimension;

		rPrimeColumn = columnAux - rColumn * subMatricesDimension;
		rPrimeRow	 = rowAux - rRow * subMatricesDimension;

		if( dimensionAux != 2 ){

			result = complexMultiply( result, R_X[ rRow ][ rColumn ] );
		}
		else{

			result = complexMultiply( result, R_X[ rowAux ][ columnAux ] );

		}

		rowAux = rPrimeRow;
		columnAux = rPrimeColumn;
		dimensionAux = subMatricesDimension;

	}

	return result;

}

void processMatrix( int dimension, Complex** matrix,  bool writeToScreen, bool writeToFile,FILE* fp, int precision ){

	Complex value;

	int rowCounter = 0;

	int columnCounter = 0;

	char* stringValue = NULL;

	for( rowCounter = 0; rowCounter < dimension; rowCounter++){

		for( columnCounter = 0; columnCounter < dimension; columnCounter++ ){

			value = calculateIndividualIndexIteratively( rowCounter, columnCounter, dimension, matrix );

			stringValue = complexNumberToString( value, precision );

			if( writeToScreen ) { printf( "%s", stringValue); }

			if( writeToFile   ){ fprintf(fp, "%s", stringValue );}

		}

		if( writeToScreen){ printf("\n"); }

		if( writeToFile   ){ fprintf(fp, "\n"); }

	}
}



void printMatrix( int dimension, Complex** matrix, int precision ){

	processMatrix( dimension, matrix,  true, false, NULL, precision );

}

void writeToFileMatrix( int dimension, Complex** matrix, char* pathToFile, int precision  ){


	FILE *fp = fopen( pathToFile, "w+");

	if( fp != NULL ){

		processMatrix( dimension, matrix, false, true, fp, precision );

		fclose(fp);

	}else{ printf("ERROR :: File exception");}

}

/* Returns an integer in the range [0, n).
 *
 * Uses rand(), and so is affected-by/affects the same seed.
 */
int randint(int n) {
	if ((n - 1) == RAND_MAX) {
		return rand();
	} else {
		// Chop off all of the values that would cause skew...
		long end = RAND_MAX / n; // truncate skew
		assert (end > 0L);
		end *= n;
		
		// ... and ignore results from rand() that fall above that limit.
		// (Worst case the loop condition should succeed 50% of the time,
		// so we can expect to bail out of this loop pretty quickly.)
		int r;
		while ((r = rand()) >= end);
		
		return r % n;
	}
}

int main( int argc, char** argv ){
	
	double theta = M_PI / 22;

	Complex** R_X = buildR_X( theta );
	
	int dimension = 8;
	int precision = 10;
	int sampleSize = dimension * dimension;
	int row = 0;
	int column = 0;
	int i,j;
	
	Complex value = calculateIndividualIndexIteratively( row, column, dimension, R_X );
	
	char* valueString = NULL;
	
	int sampleCounter = 0;
	
	for( i=0; i < dimension; i++ ) {
		for( j=0; j < dimension; j++ ) {
		    value = calculateIndividualIndexIteratively( i, j, dimension, R_X );
		    printf("%d %d %f %f\n", i, j, value.realPart, value.imaginaryPart );
		}	
	}

	/*
	int printPrecision = 10;
	printMatrix( dimension, R_X, printPrecision );
	 */
	freeMemory( R_X );

	return 0;


}
