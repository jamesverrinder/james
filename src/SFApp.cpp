#include "SFApp.h"
#include <iostream>

SFApp::SFApp(std::shared_ptr<SFWindow> window) : fire(0), is_running(true), sf_window(window) {
  int canvas_w, canvas_h;
  SDL_GetRendererOutputSize(sf_window->getRenderer(), &canvas_w, &canvas_h);

  app_box = make_shared<SFBoundingBox>(Vector2(canvas_w, canvas_h), canvas_w, canvas_h);

  player  = make_shared<SFAsset>(SFASSET_PLAYER, sf_window);
  auto player_pos = Point2(canvas_w/2, 22);
  player -> SetPosition(player_pos);
 // number of walls
    const int number_of_walls=3;
    for(int i=0; i< number_of_walls; i++){
        // making the wall
        auto Wall  = make_shared<SFAsset>(SFASSET_WALL, sf_window);
        int xpos = (( canvas_w/ number_of_walls)*i)+60;
        auto pos  = Point2(xpos,88);// setting the poss of the bricks
        Wall ->  SetPosition(pos);
        Walls.push_back(Wall);
    }
  // max_column = total aliens going across the screen
  // max_row = total row for each alien 
  const int max_column = 20;
  const int max_row = 5;

  // start_x_pos = starting x position
  // start_y_pos = starting y position  
  int start_x_pos = 20;
  int start_y_pos = 400;

  // row = current row
  // column = current_column;
  for(int row = 0; row < max_row; row++) {
    for(int column = 0 ; column < max_column; column++) {
        auto alien = make_shared<SFAsset>(SFASSET_ALIEN, sf_window); // Makes alien

        // start_x_pos + (alien->GetBoundingBox()->GetWidth() * column) = Places alien next to each other when the column goes up
        // start_y_pos - (alien->GetBoundingBox()->GetHeight() * row) = Place the alien below the other alien when the row goes up
        auto pos = Point2( start_x_pos + (alien->GetBoundingBox()->GetWidth() * column), start_y_pos - (alien->GetBoundingBox()->GetHeight() * row));
        alien->SetPosition(pos);
        aliens.push_back(alien);
    }
  }

  auto coin = make_shared<SFAsset>(SFASSET_COIN, sf_window);
  auto pos  = Point2((canvas_w/2), 500);
  coin->SetPosition(pos);
  coins.push_back(coin);

}

SFApp::~SFApp() {
  
}


/**
 * Handle all events that come from SDL.
 * These are timer or keyboard events.
 */
void SFApp::OnEvent(SFEvent& event) {
  SFEVENT the_event = event.GetCode();
  switch (the_event) {
  case SFEVENT_QUIT:
    is_running = false;
    break;
  case SFEVENT_UPDATE:
    OnUpdateWorld();
    OnRender();
    break;
  case SFEVENT_PLAYER_LEFT:
    player->GoWest();
    break;
  case SFEVENT_PLAYER_RIGHT:
    player->GoEast();
    break;
  case SFEVENT_PLAYER_UP:
    player-> GoNorth();
    break;
  case SFEVENT_PLAYER_DOWN:
    player-> GoSouth();
    break;
  case SFEVENT_FIRE:
    fire ++;
    FireProjectile();
    break;
  }
}

int SFApp::OnExecute() {
  // Execute the app
  SDL_Event event;
  while (SDL_WaitEvent(&event) && is_running) {
    // wrap an SDL_Event with our SFEvent
    SFEvent sfevent((const SDL_Event) event);
    // handle our SFEvent
    OnEvent(sfevent);
  }
}

void SFApp::OnUpdateWorld() {
  // Update projectile positions
  for(auto p: projectiles) {
    p->GoNorth();
  }

  for(auto c: coins) {
    c->GoSouth();
  }

  // Update enemy positions
  for(auto a : aliens) {
    // do something here
  }

  // Detect collisions
  for(auto p : projectiles) {
    for(auto a : aliens) {
      if(p->CollidesWith(a)) {
        p->HandleCollision();
        a->HandleCollision();

      }
    }
  }
  

  // remove dead aliens (the long way)
  list<shared_ptr<SFAsset>> tmp;
  for(auto a : aliens) {
    if(a->IsAlive()) {
      tmp.push_back(a);}
  }
  aliens.clear();
   aliens = list<shared_ptr<SFAsset>>(tmp);
   tmp.clear();

   // detect collions with projectial with alien

  for (auto p : projectiles){
    for(auto a : aliens){
      if (p->CollidesWith(a)){
      p->HandleCollision();
      }
    }
  }

//remove projectile 
 for(auto p : projectiles) {
        if(p->IsAlive()) {
       tmp.push_back(p);
       }
   }
   projectiles.clear();
 projectiles = list<shared_ptr<SFAsset>>(tmp);
 tmp.clear();

  
// Detect collisions with coins
for(auto c : coins) {
    if( player->CollidesWith(c)) {
        c->HandleCollision();
      }
    }

//remove coins  
 for(auto c : coins) {
        if(c->IsAlive()) {
       tmp.push_back(c);
       
      }
 }
 coins.clear();
coins = list<shared_ptr<SFAsset>>(tmp);
tmp.clear();

  
}
 
 void SFApp::OnRender() {
  SDL_RenderClear(sf_window->getRenderer());

  // draw the player
  player->OnRender();

 // Draw wall
  for(auto W: Walls){
    if(W->IsAlive()) { W->OnRender();}
  }

  for(auto p: projectiles) {
    if(p->IsAlive()) {p->OnRender();}
  }

  for(auto a: aliens) {
    if(a->IsAlive()) {a->OnRender();}
  }

  for(auto c: coins) {
    c->OnRender();
  }


  // Switch the off-screen buffer to be on-screen
  SDL_RenderPresent(sf_window->getRenderer());
}

void SFApp::FireProjectile() {
  auto pb = make_shared<SFAsset>(SFASSET_PROJECTILE, sf_window);
  auto v  = player->GetPosition();
  pb->SetPosition(v);
  projectiles.push_back(pb);
}
