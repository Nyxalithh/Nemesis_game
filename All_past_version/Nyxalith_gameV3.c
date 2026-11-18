//j'ai reffiner les collision (ft nyx est maintenant possible en appuyant sur la touche F XD)
#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>



typedef struct 
{
    int ax;
    int ay;

    int bx;
    int by;
}Hitbox;


typedef struct 
{
    SDL_Texture *pillier;
    SDL_Texture *sol;
    SDL_Texture *bg;

    Hitbox sol_hitbox;

    SDL_FRect rect;
}Background;


typedef struct
{
    SDL_Texture *idle1;
    SDL_Texture *idle2;

    SDL_Texture *walk1;
    SDL_Texture *walk2;
    SDL_Texture *walk3;
    SDL_Texture *walk4;
    
    
}Sprite_texture;


typedef struct 
{
    int blocked_left;
    int blocked_right;
    int onObject;

    bool isJumping;
    bool isFalling;

    int state;
}All_state;




typedef struct
{
    
    Sprite_texture textures;
    SDL_Texture *texture_actuel;
    int heading;

    Uint64 vitesseX;
    float vitesseY;
    SDL_FRect rect;
    Hitbox hitbox;

    All_state all_state;


    //int blocked_left;
    //int blocked_right;
    //int onObject;

    //bool isJumping;
    //int state;  //0 = standing,  1 = walking,   
}Sprite;

typedef struct 
{
    SDL_FRect rect;
    SDL_Texture *texture;
    Hitbox hitbox;
    char* path;
    bool onObject;
}Objet;

typedef struct 
{
    bool running;
    bool fullscreen;
    float moyenne_FPS;
    Uint32 nombreFrames;
    Uint64 debutMoyenne;
    

    SDL_Window *window;
    SDL_Renderer *renderer;

    Background background;
    Sprite nyx;
    Objet allObjets[100];
    int nbrObjet;

    Uint64 lastTime;
}App;





void keyHandle(SDL_Event *event, App *app);
void Render(App *app);
void Update (App *app, float deltaTime);

void create_object(App *app, int h, int w, int x, int y, char* path);

float gestionTemp(App *app);
void animation(App *app);

bool checkCollisionX(App *app, int i);
bool checkCollisionY(App *app, int i);

void hitboxRefrech(App *app);
void saut(App *app);
void fall(App *app);







SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    App *app = SDL_calloc(1, sizeof(App));
    if (!app)
    {
        SDL_Log("SDL_Init: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if(!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("SDL_Init: %s", SDL_GetError);
        return SDL_APP_FAILURE;
    }

    int largeur = 512;
    int hauteur = 256;
    app->running = true;

    app->window = SDL_CreateWindow("Nyxalith", largeur, hauteur, SDL_WINDOW_RESIZABLE);
    if (!app->window)
    {
        SDL_Log("SDL_CreateWindow : %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }



    app->renderer = SDL_CreateRenderer(app->window, NULL);
    if (!app->renderer)
    {
        SDL_Log("SDL_CreateWindow : %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_SetRenderLogicalPresentation(app->renderer, largeur, hauteur, SDL_LOGICAL_PRESENTATION_LETTERBOX);



    //gestion du temp
    app->lastTime = SDL_GetTicks();
    app->debutMoyenne = app->lastTime;
    app->nombreFrames = 0;
    



    //bg init
    app->background.rect.x = 0;
    app->background.rect.y = 0;
    app->background.rect.w = largeur;
    app->background.rect.h = hauteur;

    app->background.sol_hitbox.ax = -999999;
    app->background.sol_hitbox.bx = 9999999;            //grande valeurs pas atteigable. le sol doit etre continue sauf pour sa hauteur sur l'ecran.
    app->background.sol_hitbox.ay = 130;
    app->background.sol_hitbox.by = 0;                  //on peut pas etre en dessous de l'ecran (normalement)


    //nyx init
    app->nyx.rect.x = 150;
    app->nyx.rect.y = 50;
    app->nyx.rect.w = 64;
    app->nyx.rect.h = 64;
    app->nyx.vitesseX = 100.0f;
    app->nyx.vitesseY = 0.0f;

    app->nyx.all_state.state = 0;
    app->nyx.all_state.isJumping = false;
    app->nyx.all_state.isFalling = false;
    app->nyx.heading = 0;
    app->nyx.all_state.blocked_left = 0;
    app->nyx.all_state.blocked_right = 0;
/*   __________[bx,by]
    |            |
    |            |
    |            |
    |            |
  [ax,ay] _______|*/
    app->nyx.hitbox.ax = app->nyx.rect.x;
    app->nyx.hitbox.ay = app->nyx.rect.y;
    app->nyx.hitbox.bx = app->nyx.rect.x + app->nyx.rect.w;        //cree la hitbox. 
    app->nyx.hitbox.by = app->nyx.rect.y + app->nyx.rect.h;

    app->nyx.all_state.onObject = -1;



    app->nbrObjet=0;
    
    //sol init
    create_object(app, 70, 512, 0, 256-70, "none" );
    


    //objet init
    //                 h   w    x    y          path   
    create_object(app, 25, 25, 300, 160, "Asset\\objet1.bmp");
    create_object(app, 25, 50, 200, 20, "Asset\\objet1.bmp");
    create_object(app, 25, 50, 300, 100, "Asset\\objet1.bmp");
    create_object(app, 25, 70, 100, 400, "Asset\\objet1.bmp");
    create_object(app, 25, 70, 450, 50, "Asset\\objet1.bmp");
    create_object(app, 25, 50, 200, 135, "Asset\\objet1.bmp");




    



    //textures load:
    SDL_Surface *surface;


    surface = SDL_LoadBMP("Asset\\bg_bg.bmp");
    app->background.bg = SDL_CreateTextureFromSurface(app->renderer, surface);
    if(!app->background.bg)
    {
        SDL_Log("SDL_Init, 1: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    surface = SDL_LoadBMP("Asset\\bg_pillierl.bmp");
    app->background.pillier = SDL_CreateTextureFromSurface(app->renderer, surface);
    if(!app->background.pillier)
    {
        SDL_Log("SDL_Init, 2: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    surface = SDL_LoadBMP("Asset\\bg_sol.bmp");
    app->background.sol = SDL_CreateTextureFromSurface(app->renderer, surface);
    if(!app->background.sol)
    {
        SDL_Log("SDL_Init, 3: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    surface = SDL_LoadBMP("Asset\\Nyx.bmp");
    app->nyx.textures.idle1 = SDL_CreateTextureFromSurface(app->renderer, surface);
    if(!app->nyx.textures.idle1)
    {
        SDL_Log("SDL_Init, 1: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    surface = SDL_LoadBMP("Asset\\Nyx2.bmp");
    app->nyx.textures.idle2 = SDL_CreateTextureFromSurface(app->renderer, surface);
    if(!app->nyx.textures.idle2)
    {
        SDL_Log("SDL_Init, 1: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }


    

    surface = SDL_LoadBMP("Asset\\NyxWalk (1).bmp");
    app->nyx.textures.walk1 = SDL_CreateTextureFromSurface(app->renderer, surface);
    if(!app->nyx.textures.walk1)
    {
        SDL_Log("SDL_Init, 1: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    surface = SDL_LoadBMP("Asset\\NyxWalk (2).bmp");
    app->nyx.textures.walk2 = SDL_CreateTextureFromSurface(app->renderer, surface);
    if(!app->nyx.textures.walk2)
    {
        SDL_Log("SDL_Init, 1: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    surface = SDL_LoadBMP("Asset\\NyxWalk (3).bmp");
    app->nyx.textures.walk3 = SDL_CreateTextureFromSurface(app->renderer, surface);
    if(!app->nyx.textures.walk3)
    {
        SDL_Log("SDL_Init, 1: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    surface = SDL_LoadBMP("Asset\\NyxWalk (4).bmp");
    app->nyx.textures.walk4 = SDL_CreateTextureFromSurface(app->renderer, surface);
    if(!app->nyx.textures.walk4)
    {
        SDL_Log("SDL_Init, 1: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }



    for (int i = 0 ; i < app->nbrObjet ; i++)
    {
        if (app->allObjets[i].path != "none")
        {
            surface = SDL_LoadBMP(app->allObjets[i].path);
            app->allObjets[i].texture = SDL_CreateTextureFromSurface(app->renderer, surface);
            if(!app->allObjets[i].texture)
            {
                SDL_Log("SDL_Init, %d: %s", i, SDL_GetError());
                return SDL_APP_FAILURE;
            }
        }
        
    }
    



    SDL_DestroySurface(surface);


    SDL_SetTextureScaleMode(app->background.bg, SDL_SCALEMODE_NEAREST);
    SDL_SetTextureScaleMode(app->background.sol, SDL_SCALEMODE_NEAREST);
    SDL_SetTextureScaleMode(app->background.pillier, SDL_SCALEMODE_NEAREST);

    SDL_SetTextureScaleMode(app->nyx.textures.idle1, SDL_SCALEMODE_NEAREST);
    SDL_SetTextureScaleMode(app->nyx.textures.idle2, SDL_SCALEMODE_NEAREST);
     
    SDL_SetTextureScaleMode(app->nyx.textures.walk1, SDL_SCALEMODE_NEAREST);
    SDL_SetTextureScaleMode(app->nyx.textures.walk2, SDL_SCALEMODE_NEAREST);
    SDL_SetTextureScaleMode(app->nyx.textures.walk3, SDL_SCALEMODE_NEAREST);
    SDL_SetTextureScaleMode(app->nyx.textures.walk4, SDL_SCALEMODE_NEAREST);

    app->nyx.texture_actuel = app->nyx.textures.idle1;      //set texture_actuel
 

    *appstate = app;
    
    if (app->running)
    {
        return SDL_APP_CONTINUE;
    }else{
        return SDL_APP_SUCCESS;
    }
}




SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    
    App *app = appstate;

    

    switch (event->type)
    {
    case SDL_EVENT_QUIT:
        app->running = false;
        break;
    
    case SDL_EVENT_KEY_DOWN:
        keyHandle(event, app);
        break;

    default:
        break;
    }

    
    if (app->running)
    {
        return SDL_APP_CONTINUE;
    }else{
        return SDL_APP_SUCCESS;
    }
    
}


SDL_AppResult SDL_AppIterate(void *appstate)
{
    App *app = appstate;
    
    //gestion du temp.
    Uint64 frameStart = SDL_GetTicks();
    float deltaTime = gestionTemp(app);


    hitboxRefrech(app);

    //appelle des fonction
    
    Update (app, deltaTime);
    Render(app);
    
    Uint64 FrameTime = SDL_GetTicks() - frameStart;     //bloque a 60FPS environ
    if (FrameTime < 16)
    {
        SDL_Delay(16-FrameTime);
    }


    if (app->running)
    {
        return SDL_APP_CONTINUE;
    }else{
        return SDL_APP_SUCCESS;
    }
}




//------------- MY FUNCTIONS --------------------------------------------

void keyHandle(SDL_Event *event, App *app)
{
    switch (event->key.scancode)
    {
        case SDL_SCANCODE_ESCAPE:
            app->running = false;
        
        case SDL_SCANCODE_F11:
            app->fullscreen = !app->fullscreen;
            SDL_SetWindowFullscreen(app->window, app->fullscreen);
            break;
        
        case SDL_SCANCODE_SPACE:
            SDL_Log("[DEBUG] la touche saut a été presser");
            saut(app);
            app->nyx.all_state.isJumping = true;
            break;
        case SDL_SCANCODE_F:
            app->nyx.rect.h = 32;           //FAT NYX HEHE
            app->nyx.rect.w = 128;
            app->nyx.all_state.isFalling = true;
            break;
        case SDL_SCANCODE_N:
            app->nyx.rect.h = 64;           //renormal nyx
            app->nyx.rect.w = 64;
            app->nyx.all_state.isFalling = true;
            break;
        default:
            break;
    }
}

void Render(App *app)
{
    animation(app);

    SDL_SetRenderDrawColor(app->renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(app->renderer);

    if(!SDL_RenderTexture(app->renderer, app->background.bg, NULL, &app->background.rect)){
        SDL_Log("in Render(), SDL_RenderTexture: %s", SDL_GetError);
        app->running = false;
    }

    if(!SDL_RenderTexture(app->renderer, app->background.sol, NULL, &app->background.rect)){
        SDL_Log("in Render(), SDL_RenderTexture: %s", SDL_GetError);
        app->running = false;
    }
 


    if (app->nyx.heading == 0)
    {
        if(!SDL_RenderTexture(app->renderer, app->nyx.texture_actuel, NULL, &app->nyx.rect)){
            SDL_Log("in Render(), SDL_RenderTexture: %s", SDL_GetError);
            app->running = false;
        } 
    } else
    {
        if(!SDL_RenderTextureRotated(app->renderer, app->nyx.texture_actuel, NULL, &app->nyx.rect, 0, NULL, SDL_FLIP_HORIZONTAL)){
            SDL_Log("in Render(), SDL_RenderTexture: %s", SDL_GetError);
            app->running = false;
        }
    }

    
    for (int i = 0 ; i < app->nbrObjet ; i++)
    {
        if (app->allObjets[i].path != "none")
        {
            if(!SDL_RenderTexture(app->renderer, app->allObjets[i].texture, NULL, &app->allObjets[i].rect)){
                SDL_Log("in Render(),object %d, SDL_RenderTexture: %s",i, SDL_GetError);
                app->running = false;
            }
        }
    }
    



    if(!SDL_RenderTexture(app->renderer, app->background.pillier, NULL, &app->background.rect)){
        SDL_Log("in Render(), SDL_RenderTexture: %s", SDL_GetError);
        app->running = false;
    }


    SDL_RenderPresent(app->renderer);
    
}

void Update (App *app, float deltaTime)
{
    hitboxRefrech(app);
          
    const bool *keys = SDL_GetKeyboardState(NULL);
    
    
    if (keys[SDL_SCANCODE_D])
    {
        if(app->nyx.all_state.blocked_right == 0)
        {
            app->nyx.rect.x += app->nyx.vitesseX * deltaTime;
        }
        
        app->nyx.all_state.state = 1;         
        app->nyx.heading = 0;           //ou nyx regarde,     0 -> droite || 1 -> gauche
        
    }
    else if (keys[SDL_SCANCODE_A])
    {
        if(app->nyx.all_state.blocked_left == 0)
        {
            app->nyx.rect.x -= app->nyx.vitesseX * deltaTime;
        }
        
        app->nyx.all_state.state = 1;
        app->nyx.heading = 1;
    } else if (!app->nyx.all_state.isJumping)
    {
        app->nyx.all_state.state = 0;
    }

    
    
    app->nyx.rect.y += app->nyx.vitesseY * deltaTime;

    for (int i = 0 ; i < app->nbrObjet ; i++)
    {
        checkCollisionY(app, i);
        hitboxRefrech(app);
        checkCollisionX(app, i);
    }
    
    if(app->nyx.all_state.isJumping)     //si en etat de saut
    {
        saut(app);
    }
    if(app->nyx.all_state.isFalling)
    {
        fall(app);
    }

    
}

float gestionTemp(App *app)
{
    

        Uint64 currentTime= SDL_GetTicks();
        
        float tempEcouler = currentTime - app->lastTime;
        float deltaTime = tempEcouler / 1000.0f;
        float FPS = 0.0f;
        if (deltaTime > 0.0f){
            FPS = 1.0f / deltaTime;}
            
        app->nombreFrames++;
        if ((currentTime - app->debutMoyenne) >= 1000)
        {
            float secondes = (float)(currentTime - app->debutMoyenne) / 1000.0f;
            app->moyenne_FPS = app->nombreFrames / secondes;

            app->nombreFrames = 0;
            app->debutMoyenne = currentTime;

            char title[50];
            SDL_snprintf(title, sizeof(title), "Nyx's game -- FPS: %.0f", app->moyenne_FPS);
            SDL_SetWindowTitle(app->window, title);
        }
        

        app->lastTime = currentTime;

        return deltaTime;


        
}

void animation(App *app)
{
    static int n = 0;


    if (app->nyx.all_state.state == 0)
    {
        if (n < 50)
        {
            app->nyx.texture_actuel = app->nyx.textures.idle1;
            n++;
        } else if (n < 100)
        {
            app->nyx.texture_actuel = app->nyx.textures.idle2;       
            n++;
        } else {
            n = 0;
        }
    }

    if (app->nyx.all_state.state == 1)
    {

        if (n < 10)
        {
            app->nyx.texture_actuel = app->nyx.textures.walk1;
            n++;
        } else if (n < 20)
        {
            app->nyx.texture_actuel = app->nyx.textures.walk2;
            n++;
        } else if (n < 30)
        {
            app->nyx.texture_actuel = app->nyx.textures.walk3;
            n++;
        } else if (n < 40)
        {
            app->nyx.texture_actuel = app->nyx.textures.walk4;
            n++;
        } else {
            n = 0;
        }

    }
    
    
}


void AppDestroy(App *app)
{
    SDL_Log("\n[DEBUG] AppDestroy called.\n\n");
    if (!app) return;

    SDL_DestroyTexture(app->background.bg);
    SDL_DestroyTexture(app->background.sol);
    SDL_DestroyTexture(app->background.pillier);
    SDL_DestroyTexture(app->nyx.textures.idle1);
    SDL_DestroyTexture(app->nyx.textures.idle2);

    SDL_DestroyTexture(app->nyx.textures.walk1);
    SDL_DestroyTexture(app->nyx.textures.walk2);
    SDL_DestroyTexture(app->nyx.textures.walk3);
    SDL_DestroyTexture(app->nyx.textures.walk4);

    SDL_DestroyWindow(app->window);
    SDL_DestroyRenderer(app->renderer);
    SDL_free(app);
}


bool checkCollisionX(App *app, int i)
{
     if (i == 0)
    {                                   //au debut on initalise tout a 0. 
        app->nyx.all_state.blocked_left = 0;
        app->nyx.all_state.blocked_right = 0;
        
    }


    if (    app->nyx.hitbox.bx > app->allObjets[i].hitbox.ax        &&      app->nyx.hitbox.ax < app->allObjets[i].hitbox.bx        &&      app->nyx.hitbox.by > app->allObjets[i].hitbox.ay        &&       app->nyx.hitbox.ay < app->allObjets[i].hitbox.by   ) //si le coté droit de nyx est devant le cote gauche de objet et coté droit nyx derrier cote gauche
    {
        
        if (app->nyx.hitbox.ax < app->allObjets[i].hitbox.ax)      //si le coté droit de nyx est devant le cote gauche de objet
        {
            app->nyx.all_state.blocked_right += 1;
            app->nyx.rect.x = app->allObjets[i].hitbox.ax   -   app->nyx.rect.w     +      app->nyx.rect.w * (10.0f/32.0f);
            return false;
        }
        if (app->nyx.hitbox.bx > app->allObjets[i].hitbox.bx)     //si le cote gauche nyx est derrier cote droit objet
        {
            app->nyx.all_state.blocked_left += 1;
            app->nyx.rect.x = app->allObjets[i].hitbox.bx       -   app->nyx.rect.w * (10.0f/32.0f);
            return false;
        }
    }

    if (i==app->nbrObjet-1)
    {
        return true;
    }
    
   
}


bool checkCollisionY(App *app, int i)
{
    static int onObjet = -1;
    //si au moin un pixel de nyx est dans le carré (par x ou y) (sauf sur les coté, j'ai mit du mou sinon quand elle saute a coter d'un block elle est compter comme dedans)
    if (  app->nyx.hitbox.by > app->allObjets[i].hitbox.ay        &&       app->nyx.hitbox.ay < app->allObjets[i].hitbox.by     &&       app->nyx.hitbox.bx > app->allObjets[i].hitbox.ax+5        &&      app->nyx.hitbox.ax < app->allObjets[i].hitbox.bx-5     ) //si le coté droit de nyx est devant le cote gauche de objet et coté droit nyx derrier cote gauche
    {
        
        if (app->nyx.vitesseY > 0.0f               )      //si nyx decend
        {
            
            
            app->nyx.vitesseY = 0;  //on l'arrete
            app->nyx.rect.y = app->allObjets[i].hitbox.ay   - app->nyx.rect.h      +    app->nyx.rect.h * (5.0f/32.0f);     //on la met sur l'objet
            app->nyx.all_state.isJumping = false;     //elle atteris donc false
            app->nyx.all_state.isFalling = false;
            onObjet = i;        //elle est sur l'objet i

            SDL_Log("[DEBUG] on arrete nyx car elle a atteris !  vitesste y -> %f", app->nyx.vitesseY);
        }

        
        if (app->nyx.vitesseY < 0) //si nyx monte  
        {
            
            app->nyx.vitesseY = 0;      
            app->nyx.rect.y = app->allObjets[i].hitbox.by          -    app->nyx.rect.h * (9.0f/32.0f)  ;
            SDL_Log("[DEBUG] on arrete nyx car elle a touche un plafond !  vitesste y -> %f", app->nyx.vitesseY);
            
             
        }
    } 

    if (app->nyx.vitesseY == 0 && !app->nyx.all_state.isJumping)
    {
        //si nyx nest pas dans les axe de l'objet precedent (a changer d'objet)
        if (!(app->nyx.hitbox.bx > app->allObjets[onObjet].hitbox.ax        &&      app->nyx.hitbox.ax < app->allObjets[onObjet].hitbox.bx ))
        {   
            app->nyx.all_state.isFalling = true;    //on la fait decendre    
        }
        
        
    }
    
}



void create_object(App *app, int h, int w, int x, int y, char* path)
{
    
    app->allObjets[app->nbrObjet].rect.h = h;
    app->allObjets[app->nbrObjet].rect.w = w;
    app->allObjets[app->nbrObjet].rect.x = x;
    app->allObjets[app->nbrObjet].rect.y = y;

    app->allObjets[app->nbrObjet].hitbox.ax = app->allObjets[app->nbrObjet].rect.x;
    app->allObjets[app->nbrObjet].hitbox.ay = app->allObjets[app->nbrObjet].rect.y;
    app->allObjets[app->nbrObjet].hitbox.bx = app->allObjets[app->nbrObjet].rect.x + app->allObjets[app->nbrObjet].rect.w;
    app->allObjets[app->nbrObjet].hitbox.by = app->allObjets[app->nbrObjet].rect.y + app->allObjets[app->nbrObjet].rect.h;
    app->allObjets[app->nbrObjet].path = path;

    app->nbrObjet++;
}



void hitboxRefrech(App *app)
{
    app->nyx.hitbox.ax = app->nyx.rect.x +                      (app->nyx.rect.w * (10.0f/32.0f));
    app->nyx.hitbox.ay = app->nyx.rect.y +                      (app->nyx.rect.h * (9.0f/32.0f));
    app->nyx.hitbox.bx = app->nyx.rect.x + app->nyx.rect.w -    (app->nyx.rect.w * (10.0f/32.0f));         
    app->nyx.hitbox.by = app->nyx.rect.y + app->nyx.rect.h  -   (app->nyx.rect.h * (5.0f/32.0f));

    for ( int i = 0; i < app->nbrObjet ; i++)
    {
        app->allObjets[i].hitbox.ax = app->allObjets[i].rect.x;
        app->allObjets[i].hitbox.ay = app->allObjets[i].rect.y;
        app->allObjets[i].hitbox.bx = app->allObjets[i].rect.x + app->allObjets[i].rect.w;
        app->allObjets[i].hitbox.by = app->allObjets[i].rect.y + app->allObjets[i].rect.h;
    }
    

}

void saut(App *app)
{
    
    if (!app->nyx.all_state.isJumping)
    {                               //juste avant d'etre en etat de saut..
        //app->nyx.rect.y-=10;
        app->nyx.vitesseY = -300.0f;

    } else                          //en l'air..
    {
        
        app->nyx.all_state.isFalling = true;
    }
    
}

void fall(App *app)
{
    if (app->nyx.vitesseY < 300.0f)
        {
            //le probleme vient d'ici je pense. 
            app->nyx.vitesseY += 10.0f;  //ajouter deltatime apres
        }else {
            app->nyx.vitesseY = 300.0f;
            //app->nyx.isJumping = false;       //pas besoin des qu'elle atteris le jump se met a false
            
        }
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    App *app = appstate;

    AppDestroy(app);
    SDL_Quit();
}