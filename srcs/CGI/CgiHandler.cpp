#include "CGI/CgiHandler.hpp"

//? creation de 2 processus avec fork() -> le child devient un interpreteur de texte
//? puis il genere du code html via un programme (recette) python, php, bash, ...
//? il le repasse au parent qui ajoute le bon header pour la bonne reponse

CgiHandler::CgiHandler(char **env, std::string body) : _body(body), _env(env) {}

CgiHandler::~CgiHandler() {}

void CgiHandler::executeCgi() {
    int pipe_to_cgi[2];
    int pipe_from_cgi[2];
    pid_t pid = 0;

    if (pipe(pipe_to_cgi) == -1 || pipe(pipe_from_cgi) == -1) {
        //? return une erreur
    }
    pid = fork();
    if  (pid == -1) {
        close(pipe_to_cgi[0]);
        close(pipe_to_cgi[1]);
        close(pipe_from_cgi[0]);
        close(pipe_from_cgi[1]);
        //? return une erreur
    }
    else if (pid == 0) { //! chid -> oubie qu'il est un serveur et execute le script
        //? ecrit dans [1](write) et lis dans [0](read)
        //! double tableau pour execve + recuperation de l'env
        //! ecrire dans file_from_cgi[1]
        //? dup stdin dans l'ecriture de pipe_from_cgi[1];

        //! gerer les signaux en les remettant comme ils etaient definis de base
        dup2(pipe_to_cgi[0], STDIN_FILENO); //? en cas de POST on a des donnees a recup
        dup2(pipe_from_cgi[1], STDOUT_FILENO); //? ecrit ici pour que le parent puisse read
        close(pipe_to_cgi[0]);
        close(pipe_to_cgi[1]);
        close(pipe_from_cgi[0]);
        close(pipe_from_cgi[1]);
        char *args[3];
        args[0] = (char*)this->binPath.c_str();
        args[1] = (char*)this->scriptPath.c_str();
        args[2] = NULL;

        if (execve(args[0], args, env) == -1) {
            //? return une erreur, le script ne sait pas executer + free
            perror("execve error");
            exit(1);
        }
    }
    //! parent ici -> erit dans la pipe_to_cgi[1]
    //!            -> lis dans pipe_from_cgi[0]
    close(pipe_to_cgi[0]);
    close(pipe_from_cgi[1]);
    std::string body = request.getBody();
    if (!body.empty())
        write(pipe_to_cgi[1], body.c_str(), body.size());
    close(pipe_to_cgi[1]);
    char buffer[4096];
    ssize_t bits_read;
    std::string _cgi;
    while ((bits_read = read(pipe_from_cgi[0], buffer, sizeof(buffer))) > 0) {
        _cgi.append(buffer, bits_read);
    }
    if (bits_read == -1) {
        perror("read");
        //? continuer le reste ou faire un throw
    }
    close(pipe_from_cgi[0]);
    waitpid(pid, NULL, 0);
    this->response_cgi = _cgi;
}