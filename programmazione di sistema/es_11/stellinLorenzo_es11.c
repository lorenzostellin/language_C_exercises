#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/wait.h>

#define READ 0
#define WRITE 1
#define DIMM 512

int main(int argc, char *argv[])
{
    if (argc != 2)
        exit(1);

    int numArticolo, pid, p0p1[2], nread;
    char buff[DIMM];

    while (1)
    {
        // Dichiarazione dell'array di caratteri chiamato "risposta"
        char risposta[8];

        // Stampa del messaggio di richiesta input da parte dell'utente
        printf("Scrivi \"esci\" per chiudere il programma\nInserisci il numero dell’articolo che vuoi ricercare:\n-");

        // Acquisizione dell'input dell'utente attraverso la funzione "scanf"
        scanf("%s", risposta);

        // Converte tutti i caratteri dell'input inserito in minuscolo
        for (int i = 0; risposta[i] != '\0'; i++)
            risposta[i] = tolower(risposta[i]);

        // Se l'input dell'utente è "esci", viene interrotta l'esecuzione del programma
        if (strcmp("esci", risposta) == 0)
        {
            break;
        }
        // Altrimenti si continua l'esecuzione
        else
        {
            // Converte la stringa di caratteri in un intero
            int numArticolo = atoi(risposta);

            // Crea una pipe con la funzione "pipe"
            pipe(p0p1);

            // Crea un nuovo processo con la funzione "fork"
            pid = fork();

            // Se pid vale 0, il processo figlio P1 esegue un comando "grep" sul file di input specificato dall'utente
            if (pid == 0)
            {
                // Chiude i file descriptor inutilizzati
                close(p0p1[READ]);
                close(WRITE);

                // Riconfigura l'output del comando "grep" per la pipe usando la funzione "dup"
                dup(p0p1[WRITE]);
                close(p0p1[WRITE]);

                // Comando "grep" con argomenti specificati dall'utente e dalla funzione "sprintf"
                char argomento_grep[50];
                sprintf(argomento_grep, "(?<=ART. %d.)(?s).*(?=ART. %d.)", numArticolo, numArticolo + 1);
                execl("/usr/bin/grep", "grep", "-z", "-o", "-P", argomento_grep, argv[1], (char *)0);
                return -1;
            }
            // Se pid vale più di 0, il processo padre P0 legge l'output ottenuto dal processo figlio attraverso la pipe
            else if (pid > 0)
            {
                // Chiude il lato inutilizzato della pipe
                close(p0p1[WRITE]);

                // Legge l'output dalla pipe e lo stampa a schermo
                while ((nread = read(p0p1[READ], buff, 1)) > 0)
                {
                    printf("%s", buff);
                }

                // Chiude il lato inutilizzato della pipe
                close(p0p1[READ]);
            }
            // Se il processo non viene creato correttamente, viene stampato a schermo un messaggio di errore
            else
            {
                printf("errore durante la creazione del processo figlio\n");
                exit(1);
            }
        }
    }

    return 0;
}
