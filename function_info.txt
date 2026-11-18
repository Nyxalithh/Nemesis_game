ceci est le tout premier jeu que je fait. voici la fonction de chaque fonction:

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
    pour tout ce que s'execute une seul fois au debut (init)
    par ex cree la fenetre/textures ou set les variable des textures


void create_object(App *app, int h, int w, int x, int y, char* path);
    cree un objet en lui passant xyhw (ca fait moin de ligne de code dans init, il y en a trop.)







SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
    pour les event donc tout ce qui arrive une fois de temp en temp 
    (une touche par ex pour ouvrire un menu ou pour quitte/mettre en pleine ecran)
    appelle la fonction keyhandle

void keyHandle(SDL_Event *event, App *app);
    regarde quel touche a ete presser et fait les action en conscance








SDL_AppResult SDL_AppIterate(void *appstate)
    la boucle principale qui s'execute 60 fois par seconde
    elle appelle hitboxRefrech, render et update




void Render(App *app);
    affiche tout ce qu'il faut
    appelle:  animation



void Update (App *app, float deltaTime);
    s'occupe des deplacement/evenement qui arrive dans le jeu (gravité ect)
    appelle: hitbox hitboxRefrech, saut, fall, checkCollisionY, checkCollisionX


le reste leur fonction c'est un peu dans le nom:

float gestionTemp(App *app);
void animation(App *app);

bool checkCollisionX(App *app, int i);
bool checkCollisionY(App *app, int i);

void hitboxRefrech(App *app);
void saut(App *app);
void fall(App *app);





utilisation de l'IA dans tout le projet:
    apres la V9 -> demande de recherche de bug de collision. ( 1 prompt, aucun code copier coller.)
        probleme corriger grace a ce prompt:
            1. la collision de bug plus quand nyx entre trop fort dans un objet.
                il fallait changer l'ordre du code. deplacer X, corrigier X. deplacer Y corrigier y
                au lieu de: deplacer X et Y puis une fois par objet corriger X et y
                