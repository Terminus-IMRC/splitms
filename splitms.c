#define _FILE_OFFSET_BITS 64
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

char *progname;

void usage()
{
	fprintf(stderr, "usage: %s [-i input-file] [-o output-file]\n", progname);

	return;
}

int main(int argc, char *argv[])
{
	int c;
	char *input_filename=NULL, *output_filename=NULL;
	int input_fd, output_fd;
	ssize_t n;
	ssize_t rc, wc;
	char *rbuf, *wbuf;
	_Bool is_prior_space;
	_Bool newline_appeared;

	progname=argv[0];

        while((c=getopt(argc, argv, "h?i:o:"))!=-1)
                switch(c){
                        case 'i':
                                input_filename=optarg;
                                break;
                        case 'o':
                                output_filename=optarg;
                                break;
			case 'h':
                        case '?':
				usage();
				return 0;
                        default:
				fprintf(stderr, "%s: unknown option %c specified\n", progname, c);
				usage();
				exit(EXIT_FAILURE);
                }

        if(optind!=argc){
                fprintf(stderr, "%s: extra arguments specified\n", progname);
		usage();
		exit(EXIT_FAILURE);
        }

	rbuf=(char*)malloc(BUFFERSIZE*sizeof(char));
	if(!rbuf){
		fprintf(stderr, "%s: malloc: %ld: %s\n", progname, BUFFERSIZE, strerror(errno));
		exit(errno);
	}

	wbuf=(char*)malloc(BUFFERSIZE*sizeof(char));
	if(!wbuf){
		fprintf(stderr, "%s: malloc: %ld: %s\n", progname, BUFFERSIZE, strerror(errno));
		exit(errno);
	}

	if(!input_filename)
		input_fd=STDIN_FILENO;
	else if((!strcmp(input_filename, "-"))||(!strcmp(input_filename, "")))
		input_fd=STDIN_FILENO;
	else{
		input_fd=open(input_filename, O_RDONLY);
		if(input_fd==-1){
			fprintf(stderr, "%s: open: %s: %s\n", progname, input_filename, strerror(errno));
			exit(errno);
		}
	}

	if(!output_filename)
		output_fd=STDOUT_FILENO;
	else if((!strcmp(output_filename, "-"))||(!strcmp(output_filename, "")))
		output_fd=STDOUT_FILENO;
	else{
		output_fd=open(output_filename, O_RDWR | O_CREAT | O_TRUNC,  S_IRUSR | S_IWUSR);
		if(output_fd==-1){
			fprintf(stderr, "%s: open: %s: %s\n", progname, output_filename, strerror(errno));
			exit(errno);
		}
	}

	is_prior_space=1;
	while((n=read(input_fd, rbuf, BUFFERSIZE))>0){
		wc=0;
		newline_appeared=0;
		for(rc=0; rc<n; rc++){
			switch(rbuf[rc]){
				case ' ':
					if(is_prior_space)
						continue;
					else
						is_prior_space=1;
					break;

				case '\n':
					if(is_prior_space && wc)
						wc--;
					is_prior_space=1;
					newline_appeared=1;
					break;

				default:
					is_prior_space=0;
					break;
			}
			wbuf[wc++]=rbuf[rc];
		}
		if(!newline_appeared){
			fprintf(stderr, "%s: input line is too long for BUFFERSIZE %ld\n", progname, BUFFERSIZE);
			exit(EXIT_FAILURE);
		}
		write(output_fd, wbuf, wc);
	}

	if(n==-1){
		fprintf(stderr, "%s: read: %s: %s\n", progname, input_filename, strerror(errno));
		exit(errno);
	}

	return 0;
}
