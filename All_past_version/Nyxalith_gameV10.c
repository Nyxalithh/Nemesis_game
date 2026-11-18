// il faut faire un autre system d'animation. le current one est clanky
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
    float x;
    float y;
}Pos;


typedef struct 
{
    SDL_Texture *ciel;
    SDL_Texture *bg_3;
    SDL_Texture *bg_2;
    SDL_Texture *bg_1;
    SDL_Texture *sol;
    SDL_Texture *forground;

    Hitbox sol_hitbox;

    SDL_FRect rect_sol;
    SDL_FRect rect_ciel;
    SDL_FRect rect_bg1;
    SDL_FRect rect_bg2;
    SDL_FRect rect_bg3;
    SDL_FRect rect_forground;

    

}Background;


typedef struct
{
    
    SDL_Texture *idle[2];
    SDL_Texture *walk[4];
    SDL_Texture *jump[4];
    SDL_Texture *dash;
    SDL_Texture *fall[2];
    SDL_Texture *attack1[5];
    SDL_Texture *noAttack[2];
    
}Sprite_texture;


typedef struct 
{
    int blocked_left;
    int blocked_right;
    int onObject;

    bool isJumping;
    bool isFalling;
    bool isWalking;
    bool isAttacking;

    int state;  //0 = idle; 1 = walk; 2-4 = jump/fall; 5 = attack
}All_state;


typedef struct
{
    SDL_Texture *nemesis[4];
    SDL_Texture *quit[2];
    SDL_Texture *play[2];
    SDL_Texture *bg;
}Menu_texture;



typedef struct
{
    Pos pos;
}Camera;


typedef struct
{
    
    Sprite_texture textures;
    SDL_Texture *texture_actuel;
    int heading;

    Uint64 vitesseX;
    float vitesseY;
    SDL_FRect rect;
    Pos pos;
    Hitbox hitbox;

    All_state all_state;

}Sprite;

typedef struct 
{
    SDL_FRect rect;
    Pos pos;
    SDL_Texture *texture;
    Hitbox hitbox;
    char* path;
    bool onObject;
    
}Objet;

typedef struct 
{
    bool running;
    bool fullscreen;
    bool paused;

    float moyenne_FPS;
    Uint32 nombreFrames;
    Uint64 debutMoyenne;
    

    SDL_Window *window;
    SDL_Renderer *renderer;

    Background background;
    Sprite nyx;
    Menu_texture menu;

    Camera camera;

    int current_scene;  //0 = menu; 1 = game;

    Objet allObjets[100];
    int nbrObjet;

    

    Uint64 lastTime;
}App;





void keyHandle(SDL_Event *event, App *app);
void Render_game(App *app, float deltaTime);
void Update_game (App *app, float deltaTime);

void create_object(App *app, int h, int w, int x, int y, char* path);
SDL_Texture* create_texture(App *app, char *path);

float gestionTemp(App *app);
void animation(App *app, float deltaTime);

bool checkCollisionX(App *app, int i);
bool checkCollisionY(App *app, int i);

void hitboxRefrech(App *app);
void saut(App *app);
void fall(App *app, float deltaTime);

void background_animation(App *app);
void camera_animation(App *app);

void Menu_uid(App *app);


void auto_walk(App *app,  float deltaTime);




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
    app->fullscreen = false;
    app->paused = false;
    app->current_scene = 0;
    

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
    

    //camera init
    app->camera.pos.x = 0;
    app->camera.pos.y = 0;

    //bg init
    app->background.rect_bg1.x = 0;
    app->background.rect_bg1.y = -50;
    app->background.rect_bg1.w = 256*2*2;
    app->background.rect_bg1.h = 180*2;


    app->background.rect_bg2.x = 0;
    app->background.rect_bg2.y = -50;
    app->background.rect_bg2.w = 256*2*2;
    app->background.rect_bg2.h = 180*2;

    app->background.rect_bg3.x = 0;
    app->background.rect_bg3.y = -50;
    app->background.rect_bg3.w = 256*2*2;
    app->background.rect_bg3.h = 180*2;

    app->background.rect_ciel.x = 0;
    app->background.rect_ciel.y = -50;
    app->background.rect_ciel.w = 256*2;
    app->background.rect_ciel.h = 180*2;

    app->background.rect_sol.x = 0;
    app->background.rect_sol.y = -90;       //on met le sol plus haut prq c'est plus jolie hehe
    app->background.rect_sol.w = 256*2*2;
    app->background.rect_sol.h = 180*2;

    app->background.rect_forground.x = 0;
    app->background.rect_forground.y = -50;
    app->background.rect_forground.w = 256*2;
    app->background.rect_forground.h = 180*2;
    

    //nyx init
    
    app->nyx.pos.x = 225;
    app->nyx.pos.y = 50;
    app->nyx.rect.x = app->nyx.pos.x;
    app->nyx.rect.y = app->nyx.pos.y;

    app->nyx.rect.w = 64;
    app->nyx.rect.h = 64;
    app->nyx.vitesseX = 300.0f;
    app->nyx.vitesseY = 0.0f;

    app->nyx.all_state.state = 0;
    app->nyx.all_state.isJumping = false;
    app->nyx.all_state.isFalling = true;
    app->nyx.all_state.isWalking = false;
    app->nyx.all_state.isAttacking = false;
    app->nyx.heading = 0;
    app->nyx.all_state.blocked_left = 0;
    app->nyx.all_state.blocked_right = 0;
/* [ax,ay] _______
    |            |
    |            |
    |            |
    |            |
    |__________[bx,by]*/
    app->nyx.hitbox.ax = app->nyx.rect.x;
    app->nyx.hitbox.ay = app->nyx.rect.y;
    app->nyx.hitbox.bx = app->nyx.rect.x + app->nyx.rect.w;        //cree la hitbox. 
    app->nyx.hitbox.by = app->nyx.rect.y + app->nyx.rect.h;

    app->nyx.all_state.onObject = -1;



    app->nbrObjet=0;
    
    //sol init
    create_object(app, 70, 5120, -200, 256-27, "none" );
    


    //objet init    (bloc avec collision)
    
    //                 h   w    x    y          path   
    create_object(app, 25, 25, 350, 160, "Asset\\objet1.bmp");
    create_object(app, 25, 50, 200, 20, "Asset\\objet1.bmp");
    create_object(app, 25, 50, 300, 100, "Asset\\objet1.bmp");
    create_object(app, 25, 70, 100, 100, "Asset\\objet1.bmp");
    create_object(app, 25, 70, 450, 50, "Asset\\objet1.bmp");
    create_object(app, 25, 50, 200, 135, "Asset\\objet1.bmp");
    create_object(app, 250, 50, 30, 10, "Asset\\objet1.bmp");
    create_object(app, 25, 50, 400, 200, "Asset\\objet1.bmp");
    create_object(app, 25, 50, 70, 170, "Asset\\objet1.bmp");
    
    


    //bg create texture
    app->background.ciel = create_texture(app, "Asset\\Cyber_bg\\ciel.bmp");
    app->background.bg_3 = create_texture(app, "Asset\\Cyber_bg\\Bg3.bmp");
    app->background.bg_2 = create_texture(app, "Asset\\Cyber_bg\\Bg2.bmp");
    app->background.bg_1 = create_texture(app, "Asset\\Cyber_bg\\Bg1.bmp");
    app->background.sol = create_texture(app, "Asset\\Cyber_bg\\soll.bmp");
    app->background.forground = create_texture(app, "Asset\\Cyber_bg\\forground.bmp");


    //nyx create texture
    app->nyx.textures.idle[0] = create_texture(app, "Asset\\Nyx.bmp");
    app->nyx.textures.idle[1] = create_texture(app, "Asset\\Nyx2.bmp");

    app->nyx.textures.walk[0] = create_texture(app, "Asset\\NyxWalk (1).bmp");
    app->nyx.textures.walk[1] = create_texture(app, "Asset\\NyxWalk (2).bmp");
    app->nyx.textures.walk[2] = create_texture(app, "Asset\\NyxWalk (3).bmp");
    app->nyx.textures.walk[3] = create_texture(app, "Asset\\NyxWalk (4).bmp");
    
    app->nyx.textures.jump[0] = create_texture(app, "Asset\\NyxJumping1.bmp");
    app->nyx.textures.jump[1] = create_texture(app, "Asset\\NyxJumping2.bmp");
    app->nyx.textures.jump[2] = create_texture(app, "Asset\\NyxJumping3.bmp");
    app->nyx.textures.jump[3] = create_texture(app, "Asset\\NyxJumping4.bmp");

    app->nyx.textures.dash = create_texture(app, "Asset\\NyxDashing.bmp");

    app->nyx.textures.fall[0] = create_texture(app, "Asset\\NyxFalling1.bmp");
    app->nyx.textures.fall[1] = create_texture(app, "Asset\\NyxFalling2.bmp");

    app->nyx.textures.noAttack[0] = create_texture(app, "Asset\\noAttack1.bmp");
    app->nyx.textures.noAttack[1] = create_texture(app, "Asset\\noAttack2.bmp");

    app->nyx.textures.attack1[0] = create_texture(app, "Asset\\attack1--1.bmp");
    app->nyx.textures.attack1[1] = create_texture(app, "Asset\\attack1--2.bmp");
    app->nyx.textures.attack1[2] = create_texture(app, "Asset\\attack1--3.bmp");
    app->nyx.textures.attack1[3] = create_texture(app, "Asset\\attack1--4.bmp");
    app->nyx.textures.attack1[4] = create_texture(app, "Asset\\attack1--5.bmp");

    //menu create texture
    app->menu.bg = create_texture(app, "Asset\\menu\\menu_bg.bmp");
    app->menu.nemesis[0] = create_texture(app, "Asset\\menu\\nemesis1.bmp");
    app->menu.nemesis[1] = create_texture(app, "Asset\\menu\\nemesis2.bmp");
    app->menu.nemesis[2] = create_texture(app, "Asset\\menu\\nemesis3.bmp");
    app->menu.nemesis[3] = create_texture(app, "Asset\\menu\\nemesis4.bmp");
    app->menu.play[0] = create_texture(app, "Asset\\menu\\play_button0.bmp");


    for (int i = 0 ; i < app->nbrObjet ; i++)
    {
        if (app->allObjets[i].path != "none")
        {
            app->allObjets[i].texture = create_texture(app, app->allObjets[i].path);
        }
        
    }
    



    


    SDL_SetTextureScaleMode(app->background.ciel, SDL_SCALEMODE_NEAREST);
    SDL_SetTextureScaleMode(app->background.bg_3, SDL_SCALEMODE_NEAREST);
    SDL_SetTextureScaleMode(app->background.bg_2, SDL_SCALEMODE_NEAREST);
    SDL_SetTextureScaleMode(app->background.bg_1, SDL_SCALEMODE_NEAREST);
    SDL_SetTextureScaleMode(app->background.sol, SDL_SCALEMODE_NEAREST);
    SDL_SetTextureScaleMode(app->background.forground, SDL_SCALEMODE_NEAREST);

    SDL_SetTextureScaleMode(app->nyx.textures.idle[0], SDL_SCALEMODE_NEAREST);
    SDL_SetTextureScaleMode(app->nyx.textures.idle[1], SDL_SCALEMODE_NEAREST);
     
    SDL_SetTextureScaleMode(app->nyx.textures.walk[0], SDL_SCALEMODE_NEAREST);
    SDL_SetTextureScaleMode(app->nyx.textures.walk[1], SDL_SCALEMODE_NEAREST);
    SDL_SetTextureScaleMode(app->nyx.textures.walk[2], SDL_SCALEMODE_NEAREST);
    SDL_SetTextureScaleMode(app->nyx.textures.walk[3], SDL_SCALEMODE_NEAREST);

    SDL_SetTextureScaleMode(app->nyx.textures.jump[0], SDL_SCALEMODE_NEAREST);
    SDL_SetTextureScaleMode(app->nyx.textures.jump[1], SDL_SCALEMODE_NEAREST);
    SDL_SetTextureScaleMode(app->nyx.textures.jump[2], SDL_SCALEMODE_NEAREST);
    SDL_SetTextureScaleMode(app->nyx.textures.jump[3], SDL_SCALEMODE_NEAREST);

    SDL_SetTextureScaleMode(app->nyx.textures.fall[0], SDL_SCALEMODE_NEAREST);
    SDL_SetTextureScaleMode(app->nyx.textures.fall[1], SDL_SCALEMODE_NEAREST);

    SDL_SetTextureScaleMode(app->nyx.textures.dash, SDL_SCALEMODE_NEAREST);

    SDL_SetTextureScaleMode(app->nyx.textures.noAttack[0], SDL_SCALEMODE_NEAREST);
    SDL_SetTextureScaleMode(app->nyx.textures.noAttack[1], SDL_SCALEMODE_NEAREST);

    SDL_SetTextureScaleMode(app->nyx.textures.attack1[0], SDL_SCALEMODE_NEAREST);
    SDL_SetTextureScaleMode(app->nyx.textures.attack1[1], SDL_SCALEMODE_NEAREST);
    SDL_SetTextureScaleMode(app->nyx.textures.attack1[2], SDL_SCALEMODE_NEAREST);
    SDL_SetTextureScaleMode(app->nyx.textures.attack1[3], SDL_SCALEMODE_NEAREST);
    SDL_SetTextureScaleMode(app->nyx.textures.attack1[4], SDL_SCALEMODE_NEAREST);


    SDL_SetTextureScaleMode(app->menu.bg, SDL_SCALEMODE_NEAREST);
    SDL_SetTextureScaleMode(app->menu.nemesis[0], SDL_SCALEMODE_NEAREST);
    SDL_SetTextureScaleMode(app->menu.nemesis[1], SDL_SCALEMODE_NEAREST);
    SDL_SetTextureScaleMode(app->menu.nemesis[2], SDL_SCALEMODE_NEAREST);
    SDL_SetTextureScaleMode(app->menu.nemesis[3], SDL_SCALEMODE_NEAREST);
    SDL_SetTextureScaleMode(app->menu.play[0], SDL_SCALEMODE_NEAREST);
    SDL_SetTextureScaleMode(app->menu.play[1], SDL_SCALEMODE_NEAREST);
    SDL_SetTextureScaleMode(app->menu.quit[0], SDL_SCALEMODE_NEAREST);
    SDL_SetTextureScaleMode(app->menu.quit[1], SDL_SCALEMODE_NEAREST);

    app->nyx.texture_actuel = app->nyx.textures.idle[0];      //set texture_actuel
 

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
    
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
        app->current_scene = 1;
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

    if (app->current_scene == 0)
    {
        SDL_Log("yay");
        Menu_uid(app);
    }

    //appelle des fonction
    if (!app->paused  && app->current_scene == 1)           //pour pause le jeu, toute la logique du jeu tien dans ces 2 fonction :)
    {
        Update_game (app, deltaTime);
        Render_game(app, deltaTime);
    }
    
    
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
            if (!app->nyx.all_state.isJumping)
            {
                saut(app);
                app->nyx.all_state.state = 2;
                app->nyx.all_state.isJumping = true;
            }
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

        case SDL_SCANCODE_H://test pour voir la reaction des objet au mouvement de cam (a retirer)
            app->camera.pos.x++;
            break;

        case SDL_SCANCODE_J:
            app->camera.pos.x--;
            break;

        case SDL_SCANCODE_P:
            app->paused = !app->paused;
            break;
        
        case SDL_SCANCODE_E:
            app->nyx.all_state.state = 2;
            app->nyx.all_state.isAttacking = true;
            break;
        
        default:
            break;
    }
}

void Update_game (App *app, float deltaTime)
{
    hitboxRefrech(app);
    const bool *keys = SDL_GetKeyboardState(NULL);
    static int n = 0;
    
    if (keys[SDL_SCANCODE_D])
    {
        if(app->nyx.all_state.blocked_right == 0)
        {

            app->nyx.pos.x += app->nyx.vitesseX * deltaTime;
            app->camera.pos.x += app->nyx.vitesseX * deltaTime;

            app->background.rect_sol.x -= app->nyx.vitesseX * deltaTime;
            app->background.rect_bg1.x -= app->nyx.vitesseX * deltaTime*0.5f;
            app->background.rect_bg2.x -= app->nyx.vitesseX * deltaTime*0.3f;
            app->background.rect_bg3.x -= app->nyx.vitesseX * deltaTime*0.1f;
            //SDL_Log("nyx is blocked %d", n);

            app->nyx.all_state.isWalking = true;


            if (!app->nyx.all_state.isFalling && !app->nyx.all_state.isJumping){
            app->nyx.all_state.state = 1;
        }
        }
        
       
        app->nyx.heading = 0;           //ou nyx regarde,     0 -> droite || 1 -> gauche
        
    }
    else if (keys[SDL_SCANCODE_A])
    {
        if(app->nyx.all_state.blocked_left == 0)
        {
            app->nyx.pos.x -= app->nyx.vitesseX * deltaTime;
            app->camera.pos.x -= app->nyx.vitesseX * deltaTime;
            
            app->background.rect_sol.x += app->nyx.vitesseX * deltaTime;
            app->background.rect_bg1.x += app->nyx.vitesseX * deltaTime*0.5f;
            app->background.rect_bg2.x += app->nyx.vitesseX * deltaTime*0.3f;
            app->background.rect_bg3.x += app->nyx.vitesseX * deltaTime*0.1f;

            app->nyx.all_state.isWalking = true;

            if (!app->nyx.all_state.isFalling && !app->nyx.all_state.isJumping)
            {
                app->nyx.all_state.state = 1;
            }
        }
        
                 
        app->nyx.heading = 1;
    } else if (!app->nyx.all_state.isJumping    &&   !app->nyx.all_state.isFalling      )//&& !app->nyx.all_state.isAttacking)
    {
        app->nyx.all_state.state = 0;
        app->nyx.all_state.isWalking = false;         
    
    }else{
        app->nyx.all_state.isWalking = false;                //vu que le mouvement du bg se case sur si nyx est en state de saut, il faut le remettre a 0 meme si elle est entrain de saute
    }

    
    
    

    //mettre a jour la position de Nyx:
    app->nyx.rect.x = app->nyx.pos.x    -   app->camera.pos.x;
    app->nyx.rect.y = app->nyx.pos.y    -   app->camera.pos.y;
    


    for (int i = 0 ; i < app->nbrObjet ; i++)
    {
        app->allObjets[i].rect.x = app->allObjets[i].pos.x  -   app->camera.pos.x ;
        app->allObjets[i].rect.y = app->allObjets[i].pos.y  -   app->camera.pos.y;
  
    }
    for (int i = 0 ; i < app->nbrObjet ; i++)
    {
        hitboxRefrech(app);
        checkCollisionX(app, i);
    }
    app->nyx.pos.y += app->nyx.vitesseY * deltaTime;        //bouger la position du perso en fonction de sa vitesse Y
    for (int i = 0 ; i < app->nbrObjet ; i++)
    {
        hitboxRefrech(app);
        checkCollisionY(app, i);
    }
    




    if(app->nyx.all_state.isJumping)     //si en etat de saut
    {
        saut(app);
    }
    if(app->nyx.all_state.isFalling)
    {
        fall(app, deltaTime);
    }

    //auto_walk(app, deltaTime);   //a enlever

    
}


void Render_game(App *app, float deltaTime)
{
    animation(app, deltaTime);
    background_animation(app);
    camera_animation(app);
   

    SDL_SetRenderDrawColor(app->renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(app->renderer);

    if(!SDL_RenderTexture(app->renderer, app->background.ciel, NULL, &app->background.rect_ciel)){
        SDL_Log("in Render(), SDL_RenderTexture: %s", SDL_GetError);
        app->running = false;
    }

    if(!SDL_RenderTexture(app->renderer, app->background.bg_3, NULL, &app->background.rect_bg3)){
        SDL_Log("in Render(), SDL_RenderTexture: %s", SDL_GetError);
        app->running = false;
    }

    if(!SDL_RenderTexture(app->renderer, app->background.bg_2, NULL, &app->background.rect_bg2)){
        SDL_Log("in Render(), SDL_RenderTexture: %s", SDL_GetError);
        app->running = false;
    }

    if(!SDL_RenderTexture(app->renderer, app->background.bg_1, NULL, &app->background.rect_bg1)){
        SDL_Log("in Render(), SDL_RenderTexture: %s", SDL_GetError);
        app->running = false;
    }

    if(!SDL_RenderTexture(app->renderer, app->background.sol, NULL, &app->background.rect_sol)){
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
    


    /*
    if(!SDL_RenderTexture(app->renderer, app->background.forground, NULL, &app->background.rect_forground)){
        SDL_Log("in Render(), SDL_RenderTexture: %s", SDL_GetError);
        app->running = false;
    }
    */

    SDL_RenderPresent(app->renderer);
    
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

void animation(App *app, float deltaTime)
{
    //les state sont principalement set dans keyhandle/update()
    static float n = 0;
    n += deltaTime;
    SDL_Log("deltaTime: %f\nn: %f",deltaTime, n);
    SDL_Log("state: %d", app->nyx.all_state.state);

//idle
    if (app->nyx.all_state.state == 0)
    {
        if (n < 1)
        {
            app->nyx.texture_actuel = app->nyx.textures.idle[0];
            
        } else if (n < 2)
        {
            app->nyx.texture_actuel = app->nyx.textures.idle[1];       
           
        } else {
            n = 0;
        }
    }
//walk
    if (app->nyx.all_state.state == 1)
    {

        if (n < 0.2f)
        {
            app->nyx.texture_actuel = app->nyx.textures.walk[0];
            
        } else if (n < 0.4f)
        {
            app->nyx.texture_actuel = app->nyx.textures.walk[1];
            
        } else if (n < 0.6f)
        {
            app->nyx.texture_actuel = app->nyx.textures.walk[2];
            
        } else if (n < 0.8f)
        {
            app->nyx.texture_actuel = app->nyx.textures.walk[3];
            
        } else {
            n = 0;
        }

    }
  
    

//jump
   if (app->nyx.all_state.state == 2)
   {
        n=0;
        if (app->nyx.all_state.isJumping)
        {
            app->nyx.all_state.state = 3;
        }
        if (app->nyx.all_state.isAttacking)
        {
            app->nyx.all_state.state = 5;
        }
        
   }
   if (app->nyx.all_state.state == 3)
   {
    
        if ( n < 0.05f)
        {
            app->nyx.texture_actuel = app->nyx.textures.jump[1];
            
        } else if ( n < 0.1f)
        {
            app->nyx.texture_actuel = app->nyx.textures.jump[2];
            
        } else if ( n < 0.15f)
        {
            app->nyx.texture_actuel = app->nyx.textures.jump[3];
            
        } else{
            app->nyx.texture_actuel = app->nyx.textures.jump[3];
            app->nyx.all_state.state = 4;
        }
    }
    
    if (app->nyx.all_state.state == 4){
        if (app->nyx.vitesseY > -50)  //si nyx decend ( ou decend presque)
        {
            if (n < 0.2f)
            {
                app->nyx.texture_actuel = app->nyx.textures.fall[0];
            } else if (n < 0.4f)
            {
                app->nyx.texture_actuel = app->nyx.textures.fall[1];
            } else {
                n = 0;
            }
        }
    }

//attack
    if (app->nyx.all_state.state == 5){
        app->nyx    .texture_actuel = app->nyx.textures.attack1[3];
    }
}



void AppDestroy(App *app)
{
    SDL_Log("\n[DEBUG] AppDestroy called.\n\n");
    if (!app) return;

    SDL_DestroyTexture(app->background.ciel);
    SDL_DestroyTexture(app->background.bg_3);
    SDL_DestroyTexture(app->background.bg_2);
    SDL_DestroyTexture(app->background.bg_1);
    SDL_DestroyTexture(app->background.sol);
    SDL_DestroyTexture(app->background.forground);
    SDL_DestroyTexture(app->nyx.textures.idle[0]);
    SDL_DestroyTexture(app->nyx.textures.idle[1]);

    SDL_DestroyTexture(app->nyx.textures.walk[0]);
    SDL_DestroyTexture(app->nyx.textures.walk[1]);
    SDL_DestroyTexture(app->nyx.textures.walk[2]);
    SDL_DestroyTexture(app->nyx.textures.walk[3]);

    SDL_DestroyTexture(app->nyx.textures.jump[0]);
    SDL_DestroyTexture(app->nyx.textures.jump[1]);
    SDL_DestroyTexture(app->nyx.textures.jump[2]);
    SDL_DestroyTexture(app->nyx.textures.jump[3]);

    SDL_DestroyTexture(app->nyx.textures.dash);
    

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


    if (    app->nyx.hitbox.bx > app->allObjets[i].hitbox.ax        &&      app->nyx.hitbox.ax < app->allObjets[i].hitbox.bx        &&      app->nyx.hitbox.by > app->allObjets[i].hitbox.ay       &&       app->nyx.hitbox.ay < app->allObjets[i].hitbox.by   ) //si le coté droit de nyx est devant le cote gauche de objet et coté droit nyx derrier cote gauche
    {
        
        if (app->nyx.hitbox.ax < app->allObjets[i].hitbox.ax)      //si le coté droit de nyx est devant le cote gauche de objet
        {
            app->nyx.all_state.blocked_right += 1;
            app->nyx.pos.x = app->allObjets[i].hitbox.ax   -   app->nyx.rect.w     +      app->nyx.rect.w * (10.0f/32.0f) + 1;
            return false;
        }
        if (app->nyx.hitbox.bx > app->allObjets[i].hitbox.bx)     //si le cote gauche nyx est derrier cote droit objet
        {
            app->nyx.all_state.blocked_left += 1;
            app->nyx.pos.x = app->allObjets[i].hitbox.bx       -   app->nyx.rect.w * (10.0f/32.0f)   - 1;
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
    static int onObjet = 0;
    //si au moin un pixel de nyx est dans le carré (par x ou y) (sauf sur les coté, j'ai mit du mou sinon quand elle saute a coter d'un block elle est compter comme dedans)
    if (  app->nyx.hitbox.by > app->allObjets[i].hitbox.ay        &&       app->nyx.hitbox.ay < app->allObjets[i].hitbox.by     &&       app->nyx.hitbox.bx > app->allObjets[i].hitbox.ax+3        &&      app->nyx.hitbox.ax < app->allObjets[i].hitbox.bx-3     ) //si le coté droit de nyx est devant le cote gauche de objet et coté droit nyx derrier cote gauche
    {
        
        if (app->nyx.vitesseY > 0.0f               )      //si nyx decend
        {
            
            
            app->nyx.vitesseY = 0;  //on l'arrete
            app->nyx.pos.y = app->allObjets[i].hitbox.ay   - app->nyx.rect.h      +    app->nyx.rect.h * (5.0f/32.0f);     //on la met sur l'objet
            app->nyx.all_state.isJumping = false;     //elle atteris donc false
            app->nyx.all_state.isFalling = false;
            onObjet = i;        //elle est sur l'objet i

        }

        
        if (app->nyx.vitesseY < 0) //si nyx monte  
        {
            
            app->nyx.vitesseY = 0;      
            app->nyx.pos.y = app->allObjets[i].hitbox.by          -    app->nyx.rect.h * (9.0f/32.0f)  ;
            
             
        }
    } 

    if (app->nyx.vitesseY == 0 && !app->nyx.all_state.isJumping)
    {
        //si nyx nest pas dans les axe de l'objet precedent (a changer d'objet)
        if (!(app->nyx.hitbox.bx > app->allObjets[onObjet].hitbox.ax+3        &&      app->nyx.hitbox.ax < app->allObjets[onObjet].hitbox.bx-3 ))
        {   
            app->nyx.all_state.isFalling = true;    //on la fait decendre
            app->nyx.all_state.state = 4;       //on active l'animation de chute    
        }
        
        
    }
    
}

void auto_walk(App *app,  float deltaTime){
    app->nyx.vitesseX = 10.0f;
    app->nyx.pos.x += app->nyx.vitesseX * deltaTime;
            app->camera.pos.x += app->nyx.vitesseX * deltaTime;

            app->background.rect_sol.x -= app->nyx.vitesseX * deltaTime;
            app->background.rect_bg1.x -= app->nyx.vitesseX * deltaTime*0.5f;
            app->background.rect_bg2.x -= app->nyx.vitesseX * deltaTime*0.3f;
            app->background.rect_bg3.x -= app->nyx.vitesseX * deltaTime*0.1f;
            

            app->nyx.all_state.isWalking = true;
}

void create_object(App *app, int h, int w, int x, int y, char* path)
{
    
    app->allObjets[app->nbrObjet].rect.h = h;
    app->allObjets[app->nbrObjet].rect.w = w;
    app->allObjets[app->nbrObjet].rect.x = x;
    app->allObjets[app->nbrObjet].rect.y = y;

    app->allObjets[app->nbrObjet].pos.x = app->allObjets[app->nbrObjet].rect.x;
    app->allObjets[app->nbrObjet].pos.y = app->allObjets[app->nbrObjet].rect.y;

    app->allObjets[app->nbrObjet].hitbox.ax = app->allObjets[app->nbrObjet].rect.x;
    app->allObjets[app->nbrObjet].hitbox.ay = app->allObjets[app->nbrObjet].rect.y;
    app->allObjets[app->nbrObjet].hitbox.bx = app->allObjets[app->nbrObjet].rect.x + app->allObjets[app->nbrObjet].rect.w;
    app->allObjets[app->nbrObjet].hitbox.by = app->allObjets[app->nbrObjet].rect.y + app->allObjets[app->nbrObjet].rect.h;
    app->allObjets[app->nbrObjet].path = path;

    app->nbrObjet++;
}

SDL_Texture* create_texture(App *app, char *path)
{
    SDL_Texture *texture;
    SDL_Surface *surface = SDL_LoadBMP(path);
    texture = SDL_CreateTextureFromSurface(app->renderer, surface);
    if(!texture)
    {
        SDL_Log("SDL_Init, 1: %s", SDL_GetError());
        app->running = false;
    }
    return texture;

}


void background_animation(App *app)
{
    //SDL_Log("nyx pos: %f\nyx rect: %f\n\n", app->nyx.pos.x, app->nyx.rect.x);
  
  
  
    //coter gauche  
    if (app->background.rect_sol.x > 0)
    {
        app->background.rect_sol.x -= app->background.rect_sol.w/2.0f;
    }
    if (app->background.rect_bg1.x > 0)
    {
        app->background.rect_bg1.x -= app->background.rect_bg1.w/2.0f;
    }
    if (app->background.rect_bg2.x > 0)
    {
        app->background.rect_bg2.x -= app->background.rect_bg2.w/2.0f;
    }
    if (app->background.rect_bg3.x > 0)
    {
        app->background.rect_bg3.x -= app->background.rect_bg3.w/2.0f;
    }

    //coter droit
    if (app->background.rect_sol.x < app->background.rect_sol.w/2*-1)
    {
        app->background.rect_sol.x += app->background.rect_sol.w/2.0f;
    }
    if (app->background.rect_bg1.x < app->background.rect_bg1.w/2*-1)
    {
        app->background.rect_bg1.x += app->background.rect_bg1.w/2.0f;
    }
    if (app->background.rect_bg2.x < app->background.rect_bg2.w/2*-1)
    {
        app->background.rect_bg2.x += app->background.rect_bg2.w/2.0f;
    }
    if (app->background.rect_bg3.x < app->background.rect_bg3.w/2*-1)
    {
        app->background.rect_bg3.x += app->background.rect_bg3.w/2.0f;
    }

}   

void camera_animation(App *app)
{
   //SDL_Log("il faut faire bouger la camera pour mettre Nyx au centre sans toucher a son rect.");
}


void hitboxRefrech(App *app)
{
    app->nyx.hitbox.ax = app->nyx.pos.x +   (app->nyx.rect.w * (10.0f/32.0f));
    app->nyx.hitbox.ay = app->nyx.pos.y +   (app->nyx.rect.h * (9.0f/32.0f));
    app->nyx.hitbox.bx = app->nyx.pos.x + app->nyx.rect.w -    (app->nyx.rect.w * (10.0f/32.0f));         
    app->nyx.hitbox.by = app->nyx.pos.y + app->nyx.rect.h  -   (app->nyx.rect.h * (5.0f/32.0f));

    for ( int i = 0; i < app->nbrObjet ; i++)
    {
        app->allObjets[i].hitbox.ax = app->allObjets[i].pos.x;
        app->allObjets[i].hitbox.ay = app->allObjets[i].pos.y;
        app->allObjets[i].hitbox.bx = app->allObjets[i].pos.x + app->allObjets[i].rect.w;
        app->allObjets[i].hitbox.by = app->allObjets[i].pos.y + app->allObjets[i].rect.h;
    }
    

}

void saut(App *app)
{
    
    if (!app->nyx.all_state.isJumping)
    {                               //juste avant d'etre en etat de saut..
        app->nyx.vitesseY = -300.0f;

    } else                          //en l'air..
    {
        
        app->nyx.all_state.isFalling = true;
    }
    
}

void fall(App *app, float deltaTime)
{
    if (app->nyx.vitesseY < 300.0f)
        { 
            app->nyx.vitesseY += 10.0f  *   70 *   deltaTime;  //*80 car c'est environ ce qu'enleve delta time en tant normal
        }else {
            app->nyx.vitesseY = 300.0f;
            //app->nyx.isJumping = false;       //pas besoin des qu'elle atteris le jump se met a false
            
        }
}

void Menu_uid(App *app)
{
    SDL_RenderClear(app->renderer);

    if(!SDL_RenderTexture(app->renderer, app->menu.bg, NULL, &app->background.rect_ciel)){
        SDL_Log("in Render(), SDL_RenderTexture: %s", SDL_GetError);
        app->running = false;
    }

    if(!SDL_RenderTexture(app->renderer, app->menu.nemesis[3], NULL, &app->background.rect_ciel)){
        SDL_Log("in Render(), SDL_RenderTexture: %s", SDL_GetError);
        app->running = false;
    }

    if(!SDL_RenderTexture(app->renderer, app->menu.play[0], NULL, &app->background.rect_ciel)){
        SDL_Log("in Render(), SDL_RenderTexture: %s", SDL_GetError);
        app->running = false;
    }

    SDL_RenderPresent(app->renderer);


}





void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    App *app = appstate;

    AppDestroy(app);
    SDL_Quit();
}
