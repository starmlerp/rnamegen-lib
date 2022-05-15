#include <stdio.h>
#include <stdlib.h> 
#include <time.h>


#define SEPARATOR ','

#define random(x, y) (rand() % (y - x + 1) + x)

#define FSELECTOR letters[i].weight * letters[i].preceede['\0']
#define LSELECTOR letters[t].succeede[i] * letters[i].weight * letters[i].preceede[t]

struct letterdata{
	int weight, *preceede;
	int *succeede;
};

struct word{
	char letter;
	struct word *next;
};

int load(FILE* f, char ***out){
	rewind(f);
	int inp, n = 1;
	enum{
		FC, //file size countup
		WC, //word size countup
		RD  //read the words
	};
	do{
		inp = fgetc(f);
		if(inp == EOF)break;
		if(inp == SEPARATOR){
			n++;
			inp = fgetc(f);
			while(inp != ' ' && inp != EOF)inp = fgetc(f);
		}
	}while(inp != EOF);
	
	rewind(f);
	char **names = (char**)malloc(n * sizeof(char*));
	*out = names;
	int m[n];
	for(int i = 0; i < n; i++)m[i] = 0;
	n = 1;

	do{
		inp = fgetc(f);
		if(inp == EOF)break;
		if(inp == SEPARATOR || inp == '\n'){
			n++;
			inp = fgetc(f);
			while(inp != ' ' && inp != EOF)inp = fgetc(f);
		}
		else m[n-1]++;
	}while(inp != EOF);
	
	rewind(f);
	for(int i = 0; i < n; i++)names[i] = (char*)malloc((m[i] + 1) * sizeof(char));
	for(int i = 0; i < n; i++)names[i][m[i]] = '\0';
	n = 0;
	int l = 0;
	
	do{
		inp = fgetc(f);
		if(inp == EOF || inp == '\n')break;
		if(inp == SEPARATOR){
			n++;
			l = 0;
			inp = fgetc(f);
			while(inp != ' ' && inp != EOF && inp != '\n' )inp = fgetc(f);
		}
		else names[n][l++] = inp;
	}while(inp != EOF);
	return n+1;
	//TODO: use a for loop and a switch case instead of running while loop three times
}

struct letterdata *analyze(char **sample, size_t size){
	//analyze letters in the names by which letter preceeds and succeedes it
	size_t charwidth = 1;
	for(size_t i = 0; i < 8 * sizeof(char); i++)charwidth *= 2;
	struct letterdata *out = (struct letterdata*)malloc(charwidth * sizeof(struct letterdata));
	for(size_t i = 0; i < charwidth; i++){
		out[i].weight = 0;
		out[i].succeede = (int*)malloc(charwidth * sizeof(int));
		out[i].preceede = (int*)malloc(charwidth * sizeof(int));
		for(size_t j = 0; j < charwidth; j++)out[i].succeede[j] = 0;
		for(size_t j = 0; j < charwidth; j++)out[i].preceede[j] = 0;
	}
	for(size_t i = 0; i < size; i++){
		int c = -1;
		do{
			c++;
			char letter = sample[i][c];
			out[sample[i][c]].weight++;
			if(sample[i][c])out[sample[i][c]].succeede[sample[i][c+1]]++;
			if(!c){
				out[sample[i][c]].preceede['\0']++;
//				out['\0'].weight++;
//				out['\0'].succeede[sample[i][c]]++;
			}
			else out[sample[i][c]].preceede[sample[i][c-1]]++;
		}while(sample[i][c] != '\0');
	}
	out['\0'].weight/=8;
	return out;
}

char *assemble(struct letterdata *letters){
	//assemble a random word based on the letter data
	//TODO: make the assembler better utilize the data it has in function construction
	long sum = 0;
	size_t charwidth = 1, s = 0;
	for(size_t i = 0; i < 8 * sizeof(char); i++)charwidth *= 2;

	for(size_t i = 0; i < charwidth; i++)sum+=(FSELECTOR);
	struct word *f, *c = (struct word*)malloc(sizeof(struct word));
	c->next = NULL;
	f = c;
	long r = random(1, sum);
	for(size_t i = 0; i < charwidth && r > 0; i++){
		r -= (FSELECTOR);
		c->letter = i;
	}
	s++;
	while(c->letter){
		int t = c->letter;
		c->next = (struct word*)malloc(sizeof(struct word));
		c = c->next;
		c->next = NULL;
		long lsum = 0;
		for(size_t i = 0; i < charwidth; i++)lsum+=(LSELECTOR);
		long r = random(1, lsum);
//		printf("%d/%d\n", r, lsum);
		for(size_t i = 0; i < charwidth && r > 0; i++){
			c->letter = i;
			r -= (LSELECTOR);
//			printf("%d\n", r);
		}
//		printf("%c", c->letter);
		s++;
	}
	char *out = (char*)malloc(s * sizeof(char));
	c = f;
	for(size_t i = 0; i < s; i++){
		out[i] = c->letter;
		struct word *h = c;
		c = c->next;
		free(h);
	}
	return out;
}
//TODO: implement functions to load and save letter data in a file, as well as reimplement the analyzer function in order to make it possible for it to take the existing letter data, and append to it
int main(int argc, char *argv[]){
	char **names = NULL;
	int count = 8;
	if(argc == 2)count = atoi(argv[1]);
	srand(time(NULL));
	int n = load(fopen("names-2.csv", "r"), &names);
	struct letterdata *data = analyze(names, n);
/*
	for(int i = 0; i < 256; i++){
		if(data[i].weight)printf("%c(%d): %d\n", i, i, data[i].weight);
		for(int j = 0; j < 256; j++)if(data[i].succeede[j] || data[i].preceede[j])printf("\t%c(%d): %d:%d\n", j, j, data[i].preceede[j], data[i].succeede[j]); 
	}
*/
//	printf("%d\n", n);
	for(int i = 0; i < count; i++)printf("%s\n", assemble(data));
}
