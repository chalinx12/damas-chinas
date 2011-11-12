int textures[10];

typedef struct _Image
{
	unsigned int sizeX;
	unsigned int sizeY;
	char *data;
}Image;

int LoadImage1(char *filename,int index)
{
	Image *image;
	FILE *file;
   	unsigned long size;
   	unsigned long i;
    	unsigned short int planes;
    	unsigned short int bpp;
    	char temp;

    	image = (Image *) malloc(sizeof(Image));
    	if (image == NULL) 
	{
		printf("Error allocating space for image");
		exit(0);
    	}

	/* Nos aseguramos que el archivo exista y cargamos */
    	if ((file = fopen(filename, "rb"))==NULL)
    	{
		printf("File Not Found : %s\n",filename);
		return 0;
    	}

	/* Se busca la cabecera BMP */
	fseek(file, 18, SEEK_CUR);

	/* Se obtiene el ancho */
    	if ((i = fread(&image->sizeX, 4, 1, file)) != 1)
	{
		printf("Error reading width from %s.\n", filename);
		return 0;
    	}
//    	printf("Width of %s: %lu\n", filename, image->sizeX);

	/* Se obtiene el alto */
	if ((i = fread(&image->sizeY, 4, 1, file)) != 1)
	{
		printf("Error reading height from %s.\n", filename);
		return 0;
    	}
//    	printf("Height of %s: %lu\n", filename, image->sizeY);

	/* Se calcula el size asumiento que tiene 24bits/3 bytes por pixel*/
	size = image->sizeX * image->sizeY * 3;

	/* Lee los planos */
    	if ((fread(&planes, 2, 1, file)) != 1)
	{
		printf("Error reading planes from %s.\n", filename);
		return 0;
    	}

    	if (planes != 1)
	{
		printf("Planes from %s is not 1: %u\n", filename, planes);
		return 0;
    	}

	/* Lee el bpp */
    	if ((i = fread(&bpp, 2, 1, file)) != 1)
	{
		printf("Error reading bpp from %s.\n", filename);
		return 0;
    	}

    	if (bpp != 24)
	{
		printf("Bpp from %s is not 24: %u\n", filename, bpp);
		return 0;
    	}

	/* Va al resto de la cabecera bmp */
	fseek(file, 24, SEEK_CUR);

	/* Carga los datos */
	image->data = (char *) malloc(size);
    	if (image->data == NULL) 
	{
		printf("Error allocating memory for color-corrected image data");
		return 0;	
    	}

	if ((i = fread(image->data, size, 1, file)) != 1)
	{
		printf("Error reading image data from %s.\n", filename);
		return 0;
    	}

	/* Invierte los colores de bgr a rgb */
	for (i=0;i<size;i+=3)
	{
		temp = image->data[i];
		image->data[i] = image->data[i+2];
		image->data[i+2] = temp;
    	}

	/*Crea la textura */
    	glGenTextures(1, &textures[index]);
    	glBindTexture(GL_TEXTURE_2D, textures[index]);

    	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

    	glTexImage2D(GL_TEXTURE_2D, 0, 3, image->sizeX, image->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, image->data);
	free(image);
}
