// il faut faire un autre system d'animation. le current one est clanky
//j'ai fait apparaitre le cafe et fait bouger un tit gateau ! plus qu'as faire le jeu lol

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#define SOL_Y   256-27+5        //montre ou le perso marche pour que ce sois plus simple d'afficher les objet par apport au sol


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
    
    SDL_Texture *idle[4];
    SDL_Texture *walk[8];
    SDL_Texture *sprint[11];
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
    bool isSprinting;

    int state;  //0 = idle; 1 = walk; 2-4 = jump/fall; 5 = attack
}All_state;

typedef struct
{
    SDL_Texture *texture[2];
    SDL_FRect rect;
    Hitbox hitbox;
}Button;

typedef struct
{
    SDL_Texture *nemesis;

    Button achiv;
    Button setting;
    Button sound;
    Button quit;
    Button play;


    SDL_Texture *bg;

    SDL_FRect rect_nemesis;

    SDL_FRect rect_bg;

}Menu_texture;





typedef struct
{
    Pos pos;
    SDL_FRect rect;
    Hitbox hitbox;
    SDL_Texture *texture;

}Interactiv_Object;

typedef struct
{
    SDL_FRect coffee_rect;

    SDL_Texture *bg;
    SDL_Texture *tableau;
    SDL_Texture *table;
    
    SDL_Texture *cake_oven;
    Interactiv_Object blueberry_cake;
    Interactiv_Object strawberry_cake;
    Interactiv_Object raspberry_cake;
}Coffee;


typedef struct
{
    Pos pos;
}Camera;

typedef struct
{
    float x;
    float y;
}Mouse;


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
    

    Interactiv_Object cafee;
    Coffee all_coffee;


    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_FRect window_rect;

    Background background;
    Sprite mizuki;
    Menu_texture menu;

    Camera camera;
    Mouse mouse;

    int current_scene;  //0 = menu; 1 = game; 2 = cafee;
    int current_button;
    int current_interactiv; //1-19 == ville || 20-29 == cafee

    Objet allObjets[100];
    int nbrObjet;


    

    Uint64 lastTime;
}App;





void keyHandle(SDL_Event *event, App *app);
void Render_game(App *app, float deltaTime);
void Update_game (App *app, float deltaTime);

void create_object(App *app, int h, int w, int x, int y, char* path);
SDL_Texture* create_texture(App *app, char *path);
SDL_Texture* create_texturePNG(App *app, char *path);

float gestionTemp(App *app);
void animation(App *app, float deltaTime);

bool checkCollisionX(App *app, int i);
bool checkCollisionY(App *app, int i);
int checkCollisionInteractiv(App *app);

void hitboxRefrech(App *app);
void saut(App *app);
void fall(App *app, float deltaTime);

void background_animation(App *app);
void camera_animation(App *app);

void Menu_uid(App *app);
int button_check(App *app, SDL_Event *event);


int coffee_init(App* app);
int coffee_main(App* app);
void coffee_render(App *app);
void coffee_update(App *app);



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
    app->current_interactiv = 0;
    

    app->window_rect.x = 0;
    app->window_rect.y = -50;
    app->window_rect.w = 256*2;
    app->window_rect.h = 180*2;


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
    

    // mizuki init
    app->mizuki.pos.x = 225;
    app->mizuki.pos.y = 50;
    app->mizuki.rect.x = app->mizuki.pos.x;
    app->mizuki.rect.y = app->mizuki.pos.y;
    app->mizuki.rect.w = 80;
    app->mizuki.rect.h = 80;
    app->mizuki.vitesseX = 80.0f;
    app->mizuki.vitesseY = 0.0f;

    app->mizuki.all_state.state = 0;
    app->mizuki.all_state.isJumping = false;
    app->mizuki.all_state.isFalling = true;
    app->mizuki.all_state.isWalking = false;
    app->mizuki.all_state.isAttacking = false;
    app->mizuki.all_state.isSprinting = false;

    app->mizuki.heading = 0;
    app->mizuki.all_state.blocked_left = 0;
    app->mizuki.all_state.blocked_right = 0;
/* [ax,ay] _______
    |            |
    |            |
    |            |
    |            |
    |__________[bx,by]*/
    app->mizuki.hitbox.ax = app->mizuki.rect.x;
    app->mizuki.hitbox.ay = app->mizuki.rect.y;
    app->mizuki.hitbox.bx = app->mizuki.rect.x + app->mizuki.rect.w;        //cree la hitbox. 
    app->mizuki.hitbox.by = app->mizuki.rect.y + app->mizuki.rect.h;

    app->mizuki.all_state.onObject = -1;


    //menu init
    app->menu.rect_nemesis.h = 28*1.0f;
    app->menu.rect_nemesis.w = 128*1.0f;
    app->menu.rect_nemesis.x = app->background.rect_ciel.w/2+app->background.rect_ciel.w/9;
    app->menu.rect_nemesis.y = 65;

    app->menu.play.rect.h = 12*1.3f;
    app->menu.play.rect.w = 58*1.3f;
    app->menu.play.rect.x = app->background.rect_ciel.w/2+app->background.rect_ciel.w/9+        128/2   -   58*1.3f/2;
    app->menu.play.rect.y = 110;
    app->menu.play.hitbox.ax = app->menu.play.rect.x;
    app->menu.play.hitbox.ay = app->menu.play.rect.y;
    app->menu.play.hitbox.bx = app->menu.play.rect.x    +   app->menu.play.rect.w;
    app->menu.play.hitbox.by = app->menu.play.rect.y    +   app->menu.play.rect.h;


    //interactiv object init
    app->cafee.rect.h = 118 *3.0f;
    app->cafee.rect.w = 95  *3.0f;
    app->cafee.pos.x = 600;
    app->cafee.pos.y = SOL_Y - app->cafee.rect.h ;
    app->cafee.rect.x = app->cafee.pos.x;
    app->cafee.rect.y = app->cafee.pos.y;
    app->cafee.hitbox.ax = app->cafee.rect.x;
    app->cafee.hitbox.ay = app->cafee.rect.y;
    app->cafee.hitbox.bx = app->cafee.rect.x    +   app->cafee.rect.w;
    app->cafee.hitbox.by = app->cafee.rect.y    +   app->cafee.rect.h;









    app->nbrObjet=0;
    
    //sol init
    create_object(app, 70, 5120, -200, SOL_Y, "none" );  //"none"
    


    //objet init    (bloc avec collision)
    
    //                 h   w    x    y          path   
    //create_object(app, 25, 25, 350, 160, "Asset/objet1.bmp");
    create_object(app, 10, 50, 200, 20, "Asset/objet1.bmp");
    create_object(app, 10, 50, 300, 100, "Asset/objet1.bmp");
    create_object(app, 10, 70, 100, 100, "Asset/objet1.bmp");
    create_object(app, 10, 70, 450, 50, "Asset/objet1.bmp");
    create_object(app, 10, 50, 200, 135, "Asset/objet1.bmp");
    create_object(app, 250, 50, 30, 10, "Asset/objet1.bmp");
    create_object(app, 10, 50, 400, 240, "Asset/objet1.bmp");
    create_object(app, 10, 50, 70, 200, "Asset/objet1.bmp");
    
    


    //bg create texture
    app->background.ciel = create_texture(app, "Asset/Cyber_bg/ciel.bmp");
    app->background.bg_3 = create_texture(app, "Asset/Cyber_bg/Bg3.bmp");
    app->background.bg_2 = create_texture(app, "Asset/Cyber_bg/Bg2.bmp");
    app->background.bg_1 = create_texture(app, "Asset/Cyber_bg/Bg1.bmp");
    app->background.sol = create_texture(app, "Asset/Cyber_bg/soll.bmp");
    app->background.forground = create_texture(app, "Asset/Cyber_bg/forground.bmp");



    //mizuki create texture
    app->mizuki.textures.idle[0] = create_texture(app, "Asset/mizuki/idle0.bmp");
    app->mizuki.textures.idle[1] = create_texture(app, "Asset/mizuki/idle1.bmp");
    app->mizuki.textures.idle[2] = create_texture(app, "Asset/mizuki/idle2.bmp");
    app->mizuki.textures.idle[3] = create_texture(app, "Asset/mizuki/idle3.bmp");

    app->mizuki.textures.walk[0] = create_texture(app, "Asset/mizuki/walk0.bmp");
    app->mizuki.textures.walk[1] = create_texture(app, "Asset/mizuki/walk1.bmp");
    app->mizuki.textures.walk[2] = create_texture(app, "Asset/mizuki/walk2.bmp");
    app->mizuki.textures.walk[3] = create_texture(app, "Asset/mizuki/walk3.bmp");
    app->mizuki.textures.walk[4] = create_texture(app, "Asset/mizuki/walk4.bmp");
    app->mizuki.textures.walk[5] = create_texture(app, "Asset/mizuki/walk5.bmp");
    app->mizuki.textures.walk[6] = create_texture(app, "Asset/mizuki/walk6.bmp");
    app->mizuki.textures.walk[7] = create_texture(app, "Asset/mizuki/walk7.bmp");
    
    app->mizuki.textures.jump[0] = create_texture(app, "Asset/mizuki/idle0.bmp");
    app->mizuki.textures.jump[1] = create_texture(app, "Asset/mizuki/idle0.bmp");
    app->mizuki.textures.jump[2] = create_texture(app, "Asset/mizuki/idle0.bmp");
    app->mizuki.textures.jump[3] = create_texture(app, "Asset/mizuki/idle0.bmp");

    //app->mizuki.textures.dash = create_texture(app, "Asset/NyxDashing.bmp");

    app->mizuki.textures.fall[0] = create_texture(app, "Asset/mizuki/idle0.bmp");
    app->mizuki.textures.fall[1] = create_texture(app, "Asset/mizuki/idle0.bmp");

    /*
    app->mizuki.textures.noAttack[0] = create_texture(app, "Asset/noAttack1.bmp");
    app->mizuki.textures.noAttack[1] = create_texture(app, "Asset/noAttack2.bmp");

    app->mizuki.textures.attack1[0] = create_texture(app, "Asset/attack1--1.bmp");
    app->mizuki.textures.attack1[1] = create_texture(app, "Asset/attack1--2.bmp");
    app->mizuki.textures.attack1[2] = create_texture(app, "Asset/attack1--3.bmp");
    app->mizuki.textures.attack1[3] = create_texture(app, "Asset/attack1--4.bmp");
    app->mizuki.textures.attack1[4] = create_texture(app, "Asset/attack1--5.bmp");
*/
    app->mizuki.textures.sprint[0] = create_texture(app, "Asset/mizuki/sprint0.bmp");
    app->mizuki.textures.sprint[1] = create_texture(app, "Asset/mizuki/sprint1.bmp");
    app->mizuki.textures.sprint[2] = create_texture(app, "Asset/mizuki/sprint2.bmp");
    app->mizuki.textures.sprint[3] = create_texture(app, "Asset/mizuki/sprint3.bmp");
    app->mizuki.textures.sprint[4] = create_texture(app, "Asset/mizuki/sprint4.bmp");
    app->mizuki.textures.sprint[5] = create_texture(app, "Asset/mizuki/sprint5.bmp");
    app->mizuki.textures.sprint[6] = create_texture(app, "Asset/mizuki/sprint6.bmp");
    app->mizuki.textures.sprint[7] = create_texture(app, "Asset/mizuki/sprint7.bmp");
    app->mizuki.textures.sprint[8] = create_texture(app, "Asset/mizuki/sprint8.bmp");
    app->mizuki.textures.sprint[9] = create_texture(app, "Asset/mizuki/sprint9.bmp");
    app->mizuki.textures.sprint[10] = create_texture(app, "Asset/mizuki/sprint10.bmp");


    //immeuble/intereactiv object init
    app->cafee.texture = create_texture(app, "Asset/Cyber_bg/cafee.bmp");





    //menu create texture
    app->menu.bg = create_texturePNG(app, "Asset/menu/bg.png");
    app->menu.nemesis = create_texturePNG(app, "Asset/menu/Nemesis.png");
    
    app->menu.play.texture[0] = create_texturePNG(app, "Asset/menu/play_button.png");
    app->menu.play.texture[1] = create_texturePNG(app, "Asset/menu/play_button.png");
    /*
    app->menu.quit[0] = create_texturePNG(app, "Asset/menu/quit.png");
    app->menu.quit[1] = create_texturePNG(app, "Asset/menu/quit.png");
    app->menu.achiv = create_texturePNG(app, "Asset/menu/achievment.png");
    app->menu.setting = create_texturePNG(app, "Asset/menu/settings.png");
    app->menu.sound = create_texturePNG(app, "Asset/menu/sound.png");
    */

    for (int i = 0 ; i < app->nbrObjet ; i++)
    {
        if (app->allObjets[i].path != "none")
        {
            app->allObjets[i].texture = create_texture(app, app->allObjets[i].path);
        }
        
    }
    


    app->mizuki.texture_actuel = app->mizuki.textures.idle[0];      //set texture_actuel
 

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
        switch (app->current_button)
        {
        case 1:
            app->current_scene = 1;
            break;
        
        default:
            break;
        }
        break;


    case SDL_EVENT_MOUSE_MOTION:
        float x,y;
        SDL_RenderCoordinatesFromWindow(app->renderer, event->motion.x, event->motion.y,  &x,  &y);
        app->mouse.x = x;
        app->mouse.y = y;
        app->current_button = button_check(app, event);
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
        Menu_uid(app);
    }

    //appelle des fonction
    if (!app->paused  && app->current_scene == 1)           //pour pause le jeu, toute la logique du jeu tien dans ces 2 fonction :)
    {
        Update_game (app, deltaTime);
        Render_game(app, deltaTime);

    } else if (!app->paused  && app->current_scene == 2)           //mode coffee shop !!
    {
        coffee_main(app);
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
            if (!app->mizuki.all_state.isJumping)
            {
                saut(app);
                app->mizuki.all_state.state = 2;
                app->mizuki.all_state.isJumping = true;
            }
            break;


        case SDL_SCANCODE_F:
            app->mizuki.rect.h = 32;           //FAT NYX HEHE
            app->mizuki.rect.w = 128;
            app->mizuki.all_state.isFalling = true;
            break;
        case SDL_SCANCODE_N:
            app->mizuki.rect.h = 64;           //renormal mizuki
            app->mizuki.rect.w = 64;
            app->mizuki.all_state.isFalling = true;
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
            if (app->current_interactiv == 1)   //si on est vers le cafee
            {
                coffee_init(app); //on va appeler coffee init
                app->current_interactiv = 0;    //on enleve l'interactiv pour etre sur de pas appeler 2 fois coffee init
            }
            break;

        case SDL_SCANCODE_I:
            app->mizuki.all_state.state = 2;
            app->mizuki.all_state.isAttacking = true;
            break;

        case SDL_SCANCODE_LCTRL:

        if (!app->mizuki.all_state.isSprinting){
            app->mizuki.vitesseX = 300.0f;
        } else{
            app->mizuki.vitesseX = 80.0f;
        }
        
        app->mizuki.all_state.isSprinting = !app->mizuki.all_state.isSprinting;
        break;
        
        default:
            break;
    }
}

void Update_game (App *app, float deltaTime)
{
    hitboxRefrech(app);
    app->current_interactiv = checkCollisionInteractiv(app);
    SDL_Log("inter = %d", app->current_interactiv);

    const bool *keys = SDL_GetKeyboardState(NULL);
    static int n = 0;
    
    if (keys[SDL_SCANCODE_D])
    {
        if(app->mizuki.all_state.blocked_right == 0)
        {

            app->mizuki.pos.x += app->mizuki.vitesseX * deltaTime;
            app->camera.pos.x += app->mizuki.vitesseX * deltaTime;

            app->background.rect_sol.x -= app->mizuki.vitesseX * deltaTime;
            app->background.rect_bg1.x -= app->mizuki.vitesseX * deltaTime*0.5f;
            app->background.rect_bg2.x -= app->mizuki.vitesseX * deltaTime*0.3f;
            app->background.rect_bg3.x -= app->mizuki.vitesseX * deltaTime*0.1f;
            //SDL_Log("mizuki is blocked %d", n);

            app->mizuki.all_state.isWalking = true;


            if (!app->mizuki.all_state.isFalling && !app->mizuki.all_state.isJumping){
            app->mizuki.all_state.state = 1;
        }
        }
        
       
        app->mizuki.heading = 0;           //ou mizuki regarde,     0 -> droite || 1 -> gauche
        
    }
    else if (keys[SDL_SCANCODE_A])
    {
        if(app->mizuki.all_state.blocked_left == 0)
        {
            app->mizuki.pos.x -= app->mizuki.vitesseX * deltaTime;
            app->camera.pos.x -= app->mizuki.vitesseX * deltaTime;
            
            app->background.rect_sol.x += app->mizuki.vitesseX * deltaTime;
            app->background.rect_bg1.x += app->mizuki.vitesseX * deltaTime*0.5f;
            app->background.rect_bg2.x += app->mizuki.vitesseX * deltaTime*0.3f;
            app->background.rect_bg3.x += app->mizuki.vitesseX * deltaTime*0.1f;

            app->mizuki.all_state.isWalking = true;

            if (!app->mizuki.all_state.isFalling && !app->mizuki.all_state.isJumping)
            {
                app->mizuki.all_state.state = 1;
            }
        }
        
                 
        app->mizuki.heading = 1;
    } else if (!app->mizuki.all_state.isJumping    &&   !app->mizuki.all_state.isFalling      )//&& !app->mizuki.all_state.isAttacking)
    {
        app->mizuki.all_state.state = 0;
        app->mizuki.all_state.isWalking = false;         
    
    }else{
        app->mizuki.all_state.isWalking = false;                //vu que le mouvement du bg se case sur si mizuki est en state de saut, il faut le remettre a 0 meme si elle est entrain de saute
    }

    
    
    

    //mettre a jour la position de Nyx:
    app->mizuki.rect.x = app->mizuki.pos.x    -   app->camera.pos.x;
    app->mizuki.rect.y = app->mizuki.pos.y    -   app->camera.pos.y;
    
    //mettre a jour la pos des objet

    for (int i = 0 ; i < app->nbrObjet ; i++)
    {
        app->allObjets[i].rect.x = app->allObjets[i].pos.x  -   app->camera.pos.x ;
        app->allObjets[i].rect.y = app->allObjets[i].pos.y  -   app->camera.pos.y;
  
    }

    app->cafee.rect.x = app->cafee.pos.x    -   app->camera.pos.x;
    app->cafee.rect.y = app->cafee.pos.y    -   app->camera.pos.y;


    for (int i = 0 ; i < app->nbrObjet ; i++)
    {
        hitboxRefrech(app);
        checkCollisionX(app, i);
    }
    app->mizuki.pos.y += app->mizuki.vitesseY * deltaTime;        //bouger la position du perso en fonction de sa vitesse Y
    for (int i = 0 ; i < app->nbrObjet ; i++)
    {
        hitboxRefrech(app);
        checkCollisionY(app, i);
    }
    




    if(app->mizuki.all_state.isJumping)     //si en etat de saut
    {
        saut(app);
    }
    if(app->mizuki.all_state.isFalling)
    {
        fall(app, deltaTime);
    }


    
}


void Render_game(App *app, float deltaTime)
{
    animation(app, deltaTime);
    background_animation(app);
    camera_animation(app);
   

    SDL_SetRenderDrawColor(app->renderer, 0x5, 0x0, 0x10, 0xFF);
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
 
    if(!SDL_RenderTexture(app->renderer, app->cafee.texture , NULL, &app->cafee.rect)){
        SDL_Log("in Render(), SDL_RenderTexture: %s", SDL_GetError);
        app->running = false;
    }
 

    if (app->mizuki.heading == 0)
    {
        if(!SDL_RenderTexture(app->renderer, app->mizuki.texture_actuel, NULL, &app->mizuki.rect)){
            SDL_Log("in Render(), SDL_RenderTexture: %s", SDL_GetError);
            app->running = false;
        } 
    } else
    {
        if(!SDL_RenderTextureRotated(app->renderer, app->mizuki.texture_actuel, NULL, &app->mizuki.rect, 0, NULL, SDL_FLIP_HORIZONTAL)){
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
    #define idle_speed 1
    #define walk_speed 0.13f
    #define sprint_speed 0.07f
    #define jump_speed 0.05f
    #define fall_speed 0.2f


    //les state sont principalement set dans keyhandle/update()
    static float n = 0;
    n += deltaTime;
    
    //SDL_Log("deltaTime: %f\nn: %f",deltaTime, n);
    //SDL_Log("state: %d", app->mizuki.all_state.state);


    //SDL_Log("hitbox cafee\n\tax:%d\n\tbx:%d", app->cafee.hitbox.ax, app->cafee.hitbox.bx);
    //SDL_Log("hitbox mi\n\tax:%d\n\tbx:%d", app->mizuki.hitbox.ax, app->mizuki.hitbox.bx);
//idle
    
    if (app->mizuki.all_state.state == 0)
    {
        if (n < idle_speed)
        {
            app->mizuki.texture_actuel = app->mizuki.textures.idle[0];
            
        } else if (n < idle_speed*2)
        {
            app->mizuki.texture_actuel = app->mizuki.textures.idle[1];       
           
        } else {
            n = 0;
        }
    }
    
//walk
    
    if (app->mizuki.all_state.state == 1)
    {
        
        if (!app->mizuki.all_state.isSprinting)
        {
            if (n < walk_speed)
            {
                app->mizuki.texture_actuel = app->mizuki.textures.walk[0];
                
            } else if (n < walk_speed*2)
            {
                app->mizuki.texture_actuel = app->mizuki.textures.walk[1];
                
            } else if (n < walk_speed*3)
            {
                app->mizuki.texture_actuel = app->mizuki.textures.walk[2];
                
            } else if (n < walk_speed*4)
            {
                app->mizuki.texture_actuel = app->mizuki.textures.walk[3];
                
            } else if (n < walk_speed*5)
            {
                app->mizuki.texture_actuel = app->mizuki.textures.walk[4];
                
            } else if (n < walk_speed*6)
            {
                app->mizuki.texture_actuel = app->mizuki.textures.walk[5];
                
            } else if (n < walk_speed*7)
            {
                app->mizuki.texture_actuel = app->mizuki.textures.walk[6];
            }  else if (n < walk_speed*8)
            {
                app->mizuki.texture_actuel = app->mizuki.textures.walk[7];
            } else {
                n = 0;
            }
        } else 
        {
            if (n < sprint_speed)
            {
                app->mizuki.texture_actuel = app->mizuki.textures.sprint[0];
                
            } else if (n < sprint_speed*2)
            {
                app->mizuki.texture_actuel = app->mizuki.textures.sprint[1];
                
            } else if (n < sprint_speed*3)
            {
                app->mizuki.texture_actuel = app->mizuki.textures.sprint[2];
                
            } else if (n < sprint_speed*4)
            {
                app->mizuki.texture_actuel = app->mizuki.textures.sprint[3];
                
            } else if (n < sprint_speed*5)
            {
                app->mizuki.texture_actuel = app->mizuki.textures.sprint[4];
                
            } else if (n < sprint_speed*6)
            {
                app->mizuki.texture_actuel = app->mizuki.textures.sprint[5];
                
            } else if (n < sprint_speed*7)
            {
                app->mizuki.texture_actuel = app->mizuki.textures.sprint[6];
                
            }else if (n < sprint_speed*8)
            {
                app->mizuki.texture_actuel = app->mizuki.textures.sprint[7];
                
            } else if (n < sprint_speed*9)
            {
                app->mizuki.texture_actuel = app->mizuki.textures.sprint[8];
                
            } else if (n < sprint_speed*10)
            {
                app->mizuki.texture_actuel = app->mizuki.textures.sprint[9];
            }  else if (n < sprint_speed*11)
            {
                app->mizuki.texture_actuel = app->mizuki.textures.sprint[10];
            } else {
                n = 0;
            }
        }
        

    }
  
    

//jump
   if (app->mizuki.all_state.state == 2)
   {
        n=0;
        if (app->mizuki.all_state.isJumping)
        {
            app->mizuki.all_state.state = 3;
        }
        if (app->mizuki.all_state.isAttacking)
        {
            app->mizuki.all_state.state = 5;
        }
        
   }
   if (app->mizuki.all_state.state == 3)
   {
    
        if ( n < jump_speed)
        {
            app->mizuki.texture_actuel = app->mizuki.textures.jump[1];
            
        } else if ( n < jump_speed*2)
        {
            app->mizuki.texture_actuel = app->mizuki.textures.jump[2];
            
        } else if ( n < jump_speed*3)
        {
            app->mizuki.texture_actuel = app->mizuki.textures.jump[3];
            
        } else{
            app->mizuki.texture_actuel = app->mizuki.textures.jump[3];
            app->mizuki.all_state.state = 4;
        }
    }
    
    if (app->mizuki.all_state.state == 4){
        if (app->mizuki.vitesseY > -50)  //si mizuki decend ( ou decend presque)
        {
            if (n < fall_speed)
            {
                app->mizuki.texture_actuel = app->mizuki.textures.fall[0];
            } else if (n < fall_speed*2)
            {
                app->mizuki.texture_actuel = app->mizuki.textures.fall[1];
            } else {
                n = 0;
            }
        }
    }

//attack
    if (app->mizuki.all_state.state == 5){
        app->mizuki.texture_actuel = app->mizuki.textures.attack1[3];
    }
}

void animation_by_frame(App *app)
{

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
    SDL_DestroyTexture(app->mizuki.textures.idle[0]);
    SDL_DestroyTexture(app->mizuki.textures.idle[1]);

    SDL_DestroyTexture(app->mizuki.textures.walk[0]);
    SDL_DestroyTexture(app->mizuki.textures.walk[1]);
    SDL_DestroyTexture(app->mizuki.textures.walk[2]);
    SDL_DestroyTexture(app->mizuki.textures.walk[3]);

    SDL_DestroyTexture(app->mizuki.textures.jump[0]);
    SDL_DestroyTexture(app->mizuki.textures.jump[1]);
    SDL_DestroyTexture(app->mizuki.textures.jump[2]);
    SDL_DestroyTexture(app->mizuki.textures.jump[3]);

    SDL_DestroyTexture(app->mizuki.textures.dash);
    

    SDL_DestroyWindow(app->window);
    SDL_DestroyRenderer(app->renderer);
    SDL_free(app);
}

int checkCollisionInteractiv(App *app)
{
    if (app->mizuki.hitbox.bx > app->cafee.hitbox.ax        &&      app->mizuki.hitbox.ax < app->cafee.hitbox.bx        &&      app->mizuki.hitbox.by > app->cafee.hitbox.ay       &&       app->mizuki.hitbox.ay < app->cafee.hitbox.by   ) 
    {
        //faire apparaitre un indicateur qu'on peut entrer dans le batiment pkus tard
        return 1;
    }
    else{
        return 0;
    }
    
}

bool checkCollisionX(App *app, int i)
{
     if (i == 0)
    {                                   //au debut on initalise tout a 0. 
        app->mizuki.all_state.blocked_left = 0;
        app->mizuki.all_state.blocked_right = 0;
        
    }


    if (    app->mizuki.hitbox.bx > app->allObjets[i].hitbox.ax        &&      app->mizuki.hitbox.ax < app->allObjets[i].hitbox.bx        &&      app->mizuki.hitbox.by > app->allObjets[i].hitbox.ay       &&       app->mizuki.hitbox.ay < app->allObjets[i].hitbox.by   ) //si le coté droit de mizuki est devant le cote gauche de objet et coté droit mizuki derrier cote gauche
    {
        
        if (app->mizuki.hitbox.ax < app->allObjets[i].hitbox.ax)      //si le coté droit de mizuki est devant le cote gauche de objet
        {
            app->mizuki.all_state.blocked_right += 1;
            app->mizuki.pos.x = app->allObjets[i].hitbox.ax   -   app->mizuki.rect.w     +      app->mizuki.rect.w * (26.0f/80.0f) + 1;
            return false;
        }
        if (app->mizuki.hitbox.bx > app->allObjets[i].hitbox.bx)     //si le cote gauche mizuki est derrier cote droit objet
        {
            app->mizuki.all_state.blocked_left += 1;
            app->mizuki.pos.x = app->allObjets[i].hitbox.bx       -   app->mizuki.rect.w * (27.0f/80.0f)   - 1;
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
    //si au moin un pixel de mizuki est dans le carré (par x ou y) (sauf sur les coté, j'ai mit du mou sinon quand elle saute a coter d'un block elle est compter comme dedans)
    if (  app->mizuki.hitbox.by > app->allObjets[i].hitbox.ay        &&       app->mizuki.hitbox.ay < app->allObjets[i].hitbox.by     &&       app->mizuki.hitbox.bx > app->allObjets[i].hitbox.ax+3        &&      app->mizuki.hitbox.ax < app->allObjets[i].hitbox.bx-3     ) //si le coté droit de mizuki est devant le cote gauche de objet et coté droit mizuki derrier cote gauche
    {
        
        if (app->mizuki.vitesseY > 0.0f               )      //si mizuki decend
        {
            
            
            app->mizuki.vitesseY = 0;  //on l'arrete
            app->mizuki.pos.y = app->allObjets[i].hitbox.ay   - app->mizuki.rect.h      +    app->mizuki.rect.h * (2.0f/80.0f);     //on la met sur l'objet
            app->mizuki.all_state.isJumping = false;     //elle atteris donc false
            app->mizuki.all_state.isFalling = false;
            onObjet = i;        //elle est sur l'objet i

        }

        
        if (app->mizuki.vitesseY < 0) //si mizuki monte  
        {
            
            app->mizuki.vitesseY = 0;      
            app->mizuki.pos.y = app->allObjets[i].hitbox.by          -    app->mizuki.rect.h * (10.0f/80.0f)  ;
            
             
        }
    } 

    if (app->mizuki.vitesseY == 0 && !app->mizuki.all_state.isJumping)
    {
        //si mizuki nest pas dans les axe de l'objet precedent (a changer d'objet)
        if (!(app->mizuki.hitbox.bx > app->allObjets[onObjet].hitbox.ax+3        &&      app->mizuki.hitbox.ax < app->allObjets[onObjet].hitbox.bx-3 ))
        {   
            app->mizuki.all_state.isFalling = true;    //on la fait decendre
            app->mizuki.all_state.state = 4;       //on active l'animation de chute    
        }
        
        
    }
    
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
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
    return texture;

}

SDL_Texture* create_texturePNG(App *app, char *path)
{
    SDL_Texture *texture;
    SDL_Surface *surface = SDL_LoadPNG(path);
    texture = SDL_CreateTextureFromSurface(app->renderer, surface);
    if(!texture)
    {
        SDL_Log("SDL_Init, 1: %s", SDL_GetError());
        app->running = false;
    }
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
    return texture;

}



void background_animation(App *app)
{
    //SDL_Log("mizuki pos: %f\mizuki rect: %f\n\n", app->mizuki.pos.x, app->mizuki.rect.x);
  
  
  
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
    app->mizuki.hitbox.ax = app->mizuki.pos.x +   (app->mizuki.rect.w * (27.0f/80.0f));
    app->mizuki.hitbox.ay = app->mizuki.pos.y +   (app->mizuki.rect.h * (10.0f/80.0f));
    app->mizuki.hitbox.bx = app->mizuki.pos.x + app->mizuki.rect.w -    (app->mizuki.rect.w * (26.0f/80.0f));         
    app->mizuki.hitbox.by = app->mizuki.pos.y + app->mizuki.rect.h  -   (app->mizuki.rect.h * (2.0f/80.0f));

    for ( int i = 0; i < app->nbrObjet ; i++)
    {
        app->allObjets[i].hitbox.ax = app->allObjets[i].pos.x;
        app->allObjets[i].hitbox.ay = app->allObjets[i].pos.y;
        app->allObjets[i].hitbox.bx = app->allObjets[i].pos.x + app->allObjets[i].rect.w;
        app->allObjets[i].hitbox.by = app->allObjets[i].pos.y + app->allObjets[i].rect.h;
    }
    
    app->cafee.hitbox.ax = app->cafee.pos.x;
    app->cafee.hitbox.bx = app->cafee.pos.x +   app->cafee.rect.w;

}

void saut(App *app)
{
    
    if (!app->mizuki.all_state.isJumping)
    {                               //juste avant d'etre en etat de saut..
        app->mizuki.vitesseY = -200.0f;

    } else                          //en l'air..
    {
        
        app->mizuki.all_state.isFalling = true;
    }
    
}

void fall(App *app, float deltaTime)
{
    if (app->mizuki.vitesseY < 300.0f)
        { 
            app->mizuki.vitesseY += 10.0f  *   70 *   deltaTime;  //*80 car c'est environ ce qu'enleve delta time en tant normal
        }else {
            app->mizuki.vitesseY = 300.0f;
            //app->mizuki.isJumping = false;       //pas besoin des qu'elle atteris le jump se met a false
            
        }
}






void Menu_uid(App *app)
{
    SDL_SetRenderDrawColor(app->renderer, 0xFF, 0xEE, 0xFF, 0xFF);
    SDL_RenderClear(app->renderer);
    
    



    if(!SDL_RenderTexture(app->renderer, app->menu.bg, NULL, &app->window_rect)){
        SDL_Log("in Render(), SDL_RenderTexture: %s", SDL_GetError);
        app->running = false;
    }

    if(!SDL_RenderTexture(app->renderer, app->menu.nemesis, NULL, &app->menu.rect_nemesis)){
        SDL_Log("in Render(), SDL_RenderTexture: %s", SDL_GetError);
        app->running = false;
    }
    
    if(!SDL_RenderTexture(app->renderer, app->menu.play.texture[0], NULL, &app->menu.play.rect)){
        SDL_Log("in Render(), SDL_RenderTexture: %s", SDL_GetError);
        app->running = false;
    }
    

    static int counter = 0;
    counter++;
    SDL_RenderDebugText(app->renderer, 0.0f, 20.0f, "hellooo HEHEHEHEEH ;P");
    SDL_RenderDebugTextFormat(app->renderer, 0.0f, 30.0f, "idk: %d",counter);


    SDL_RenderPresent(app->renderer);


}

int button_check(App *app, SDL_Event *event)    //check sur quel button est la souris.
{
    if (app->mouse.y > app->menu.play.hitbox.ay        &&       app->mouse.y < app->menu.play.hitbox.by     &&       app->mouse.x > app->menu.play.hitbox.ax        &&      app->mouse.x < app->menu.play.hitbox.bx)
    {
        return 1;   //objet 1
    }
    return 0;
}



void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    App *app = appstate;

    AppDestroy(app);
    SDL_Quit();
}





int coffee_init(App* app)
{
    app->all_coffee.coffee_rect.h = 128*2;
    app->all_coffee.coffee_rect.w = 256*2;
    app->all_coffee.coffee_rect.x = 0;
    app->all_coffee.coffee_rect.y = 0;

    app->all_coffee.blueberry_cake.rect.h = 13*2;
    app->all_coffee.blueberry_cake.rect.w = 24*2;
    app->all_coffee.blueberry_cake.rect.x = 0;
    app->all_coffee.blueberry_cake.rect.y = 0;
    app->all_coffee.blueberry_cake.hitbox.ax = app->all_coffee.blueberry_cake.rect.x;
    app->all_coffee.blueberry_cake.hitbox.ay = app->all_coffee.blueberry_cake.rect.y;
    app->all_coffee.blueberry_cake.hitbox.bx = app->all_coffee.blueberry_cake.rect.x    +   app->all_coffee.blueberry_cake.rect.w;
    app->all_coffee.blueberry_cake.hitbox.by = app->all_coffee.blueberry_cake.rect.y    +   app->all_coffee.blueberry_cake.rect.h;




    SDL_Log("[DEBUG] coffee init!\n\tcreation des texture!");
    app->all_coffee.bg = create_texture(app, "Asset/coffee_shop/coffee_bg.bmp");
    app->all_coffee.table = create_texture(app, "Asset/coffee_shop/coffee_table.bmp");
    app->all_coffee.cake_oven = create_texture(app, "Asset/coffee_shop/cakeOven.bmp");
    app->all_coffee.tableau = create_texture(app, "Asset/coffee_shop/tafel.bmp");
    app->all_coffee.blueberry_cake.texture = create_texture(app, "Asset/coffee_shop/cake_blueberry.bmp");
    app->all_coffee.raspberry_cake.texture = create_texture(app, "Asset/coffee_shop/cake_framboise.bmp");
    app->all_coffee.strawberry_cake.texture = create_texture(app, "Asset/coffee_shop/cake_strawberry.bmp");

    app->current_scene = 2; //on passe a scene 2 donc c'est coffee main qui va se faire appeler maintenant.
}

int coffee_main(App* app)
{
    coffee_render(app);
    coffee_update(app);
}


void coffee_render(App *app)
{
    SDL_SetRenderDrawColor(app->renderer, 0x5, 0x0, 0x10, 0xFF);
    SDL_RenderClear(app->renderer);

    if(!SDL_RenderTexture(app->renderer, app->all_coffee.bg , NULL, &app->all_coffee.coffee_rect)){
        SDL_Log("in Render(), SDL_RenderTexture: %s", SDL_GetError);
        app->running = false;}

    if(!SDL_RenderTexture(app->renderer, app->all_coffee.tableau , NULL, &app->all_coffee.coffee_rect)){
        SDL_Log("in Render(), SDL_RenderTexture: %s", SDL_GetError);
        app->running = false;}

    if(!SDL_RenderTexture(app->renderer, app->all_coffee.table , NULL, &app->all_coffee.coffee_rect)){
        SDL_Log("in Render(), SDL_RenderTexture: %s", SDL_GetError);
        app->running = false;}

    if(!SDL_RenderTexture(app->renderer, app->all_coffee.cake_oven , NULL, &app->all_coffee.coffee_rect)){
        SDL_Log("in Render(), SDL_RenderTexture: %s", SDL_GetError);
        app->running = false;}
    

    if(!SDL_RenderTexture(app->renderer, app->all_coffee.blueberry_cake.texture , NULL, &app->all_coffee.blueberry_cake.rect)){
        SDL_Log("in Render(), SDL_RenderTexture: %s", SDL_GetError);
        app->running = false;}
    
    
    
    
    
    SDL_RenderPresent(app->renderer);
}

void coffee_update(App *app)
{
    app->all_coffee.blueberry_cake.rect.x = app->mouse.x    -   app->all_coffee.blueberry_cake.rect.w/2;
    app->all_coffee.blueberry_cake.rect.y = app->mouse.y    -   app->all_coffee.blueberry_cake.rect.h/2;
}