#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <math.h>
#include <iostream>
#include <list>
#include <string>

#include "constants.h"

using namespace std;
using namespace sf; 

class Animation
{
    public:
        float Frame, speed;
        Sprite sprite;
        vector<IntRect> frames;
        Animation(){}

    Animation (Texture &t, int x, int y, int w, int h, int count, float Speed)
	{
	    Frame = 0;
        speed = Speed;

		for (int i=0;i<count;i++)
        frames.push_back( IntRect(x+i*w, y, w, h)  );

		sprite.setTexture(t);
		sprite.setOrigin(w/2,h/2);
        sprite.setTextureRect(frames[0]);
	}


	void update()
	{
    	Frame += speed;
		int n = frames.size();
		if (Frame >= n) Frame -= n;
		if (n>0) sprite.setTextureRect( frames[int(Frame)] );
	}

	bool isEnd()
	{
	  return Frame+speed>=frames.size();
	}


};

class Entity
{
  public:
  float x,y,dx,dy,R,angle;
  bool life;
  std::string name;
  Animation anim;

  Entity()
  {
    life=1;
  }

  void settings(Animation &a,int X,int Y,float Angle=0,int radius=1)
  {
    anim = a;
    x=X; y=Y;
    angle = Angle;
    R = radius;
  }

  virtual void update(){};

  void draw(RenderWindow &app)
  {
    anim.sprite.setPosition(x,y);
    anim.sprite.setRotation(angle + 90);
    app.draw(anim.sprite);

    CircleShape circle(R);
    circle.setFillColor(Color(255,0,0,170));
    circle.setPosition(x,y);
    circle.setOrigin(R,R);
    //app.draw(circle);
  }

  virtual ~Entity(){};
};

class asteroid: public Entity
{
public:
  asteroid()
  {
    dx=rand()%2-1;
    dy=rand()%2-1;
    name="asteroid";
  }

void  update()
  {
   x+=dx;
   y+=dy;

   if (x>W) x=0;  if (x<0) x=W;
   if (y>H) y=0;  if (y<0) y=H;
  }
};

class bullet: public Entity
{
public:
  bullet()
  {
    name="bullet";
  }

void  update()
  {
   dx=cos(angle*DEGTORAD)*6;
   dy=sin(angle*DEGTORAD)*6;
  // angle+=rand()%6-3;
   x+=dx;
   y+=dy;

   if (x>W || x<0 || y>H || y<0) life=0;
  }

};


class player: public Entity
{
public:
   bool thrust;

   player()
   {
     name = "player";
   }

   void update()
   {
     if (thrust)
      { dx+=cos(angle*DEGTORAD)*0.2;
        dy+=sin(angle*DEGTORAD)*0.2; }
     else
      { dx*=0.99;
        dy*=0.99; }

    int maxSpeed=15;
    float speed = sqrt(dx*dx+dy*dy);
    if (speed>maxSpeed)
    {
        dx *= maxSpeed/speed;
        dy *= maxSpeed/speed; 
    }

    x+=dx;
    y+=dy;

    if (x>W) x=0; if (x<0) x=W;
    if (y>H) y=0; if (y<0) y=H;
   }

};


bool isCollide(Entity *a,Entity *b)
{
  return (b->x - a->x)*(b->x - a->x)+
         (b->y - a->y)*(b->y - a->y)<
         (a->R + b->R)*(a->R + b->R);
}


int main() {
    setlocale(LC_ALL, ""); // Поддержка кириллицы в консоли Windows
    IpAddress ip1 = IpAddress::getLocalAddress();
    IpAddress ip2 = IpAddress::getLocalAddress(); //Локальный ip1 Адресс
    TcpSocket socket1, socket2;//программный интерфейс для обеспечения обмена данными между процессами
    Packet packet1, packet2; //Для осуществления пакетной передачи дынных
    char type;
    char mode = ' ';//Мод s- сервер, с - клиент
    char buffer1[2000];
    char buffer2[2000];

    size_t received; //??
    string text = "connect to: ";

    //***********Подключение***************//
    cout << ip1 << endl;
    cout << ip2 << endl;

    cout << "Введите тип подключения: c -клиент, s -сервер" << endl;
    cin >> type;
    if(type == 's'){
    TcpListener listener;

    listener.listen(2003);
    listener.accept(socket1);

    socket2.connect(ip2, 2004); //ip1 и Порт

    //который будет содержать новое соединение
    text += "Serwer";
    mode = 's';
    }else if(type == 'c'){
        socket1.connect(ip1, 2003);

        TcpListener listener1;

        listener1.listen(2004);
        listener1.accept(socket2);

        //ip1 и Порт
        text += "client";
        mode = 'c';
    }

    socket1.send(text.c_str(), text.length() + 1);
    socket2.send(text.c_str(), text.length() + 1);

    socket1.receive(buffer1, sizeof(buffer1), received);
    socket2.receive(buffer2, sizeof(buffer2), received);

    cout << buffer1 << endl;
    cout << buffer2 << endl;

    //**********Отрисовка Формы***********************//
    RenderWindow app(VideoMode(W, H), "Game!");
    app.setFramerateLimit(60);
    //*******Элементы********************//
     Texture t1,t2,t3,t4,t5,t6,t7, t8;
    t1.loadFromFile("images/spaceship.png");
    t2.loadFromFile("images/background.jpg");
    t3.loadFromFile("images/explosions/type_C.png");
    t4.loadFromFile("images/rock.png");
    t5.loadFromFile("images/fire_blue.png");
    t6.loadFromFile("images/rock_small.png");
    t7.loadFromFile("images/explosions/type_B.png");
    t8.loadFromFile("images/spaceship.png");

    t1.setSmooth(true);
    t2.setSmooth(true);


    Sprite background(t2);


    Animation sExplosion(t3, 0,0,256,256, 48, 0.5);
    Animation sRock(t4, 0,0,64,64, 16, 0.2);
    Animation sRock_small(t6, 0,0,64,64, 16, 0.2);
    Animation sBullet(t5, 0,0,32,64, 16, 0.8);
    Animation sPlayer(t1, 40,0,40,40, 1, 0);
    Animation sPlayer_go(t1, 40,40,40,40, 1, 0);
    Animation sPlayer2(t8, 40,0,40,40, 1, 0);
    Animation sPlayer_go2(t8, 40,40,40,40, 1, 0);
    Animation sExplosion_ship(t7, 0,0,192,192, 64, 0.5);


    list<Entity*> entities;
    

    for(int i=0;i<15;i++)
    {
      asteroid *a = new asteroid();
      a->settings(sRock, rand()%W, rand()%H, rand()%360, 25);
      entities.push_back(a);
    }

    player *p = new player();
    p->settings(sPlayer,200,200,0,20);
    entities.push_back(p);

    player *p2 = new player();
    p2->settings(sPlayer,200,200,0,20);
    entities.push_back(p2);

    while(app.isOpen()){

        sf::Event event;
        while(app.pollEvent(event))
        {
            if(event.type == sf::Event::Closed){
            app.close();
        }

        }
        if(mode == 's'){//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
            socket1.receive(packet1); //Команда которая ожидает данных в виде пакета от клиентской части
            if(packet1 >> p->x >> p->y >> p->angle >> p->life){ //вытаскиваем значение из пакета в переменную x1 и у (действие уже происходит)
            cout << p->x << ":" << p->y << endl;
            }
            //управление персонажем
            if(Keyboard::isKeyPressed(Keyboard::D)) p2->angle += 3;
            if(Keyboard::isKeyPressed(Keyboard::A)) p2->angle -= 3;
            if(Keyboard::isKeyPressed(Keyboard::W)) p2->thrust = true;
            else p2->thrust = false;
            if (event.type == Event::KeyPressed){
                if (event.key.code == Keyboard::Space)
                {
                    bullet *b2 = new bullet();
                    b2->settings(sBullet,p2->x,p2->y,p2->angle,10);
                    entities.push_back(b2);
                }
            }


     for(auto a2:entities)
     for(auto b2:entities)
    {
      if (a2->name=="asteroid" && b2->name=="bullet")
       if ( isCollide(a2,b2) )
           {
            a2->life=false;
            b2->life=false;

            Entity *e2 = new Entity();
            e2->settings(sExplosion,a2->x,a2->y);
            e2->name="explosion";
            entities.push_back(e2);


            for(int i=0;i<2;i++)
            {
             if (a2->R==15) continue;
             Entity *e2 = new asteroid();
             e2->settings(sRock_small,a2->x,a2->y,rand()%360,15);
             entities.push_back(e2);
            }

           }

      if (a2->name=="player" && b2->name=="asteroid")
       if ( isCollide(a2,b2) )
           {
            b2->life=false;

            Entity *e2 = new Entity();
            e2->settings(sExplosion_ship,a2->x,a2->y);
            e2->name="explosion";
            entities.push_back(e2);

            p2->settings(sPlayer2,W/2,H/2,0,20);
            p2->dx=0; p2->dy=0;
           }
    }


    if (p2->thrust)  p2->anim = sPlayer_go;
    else   p2->anim = sPlayer;


    for(auto e2:entities)
     if (e2->name=="explosion")
      if (e2->anim.isEnd()) e2->life=0;

    if (rand()%2000000==0 && rand()%200200 == 0)
     {
       asteroid *a = new asteroid();
       a->settings(sRock, 0,rand()%H, rand()%360, 25);
       entities.push_back(a);
     }

    for(auto i=entities.begin();i!=entities.end();)
    {
      Entity *e = *i;

      e->update();
      e->anim.update();

      if (e->life==false) {i=entities.erase(i); delete e;}
      else i++;
    }

 
            packet2 << p2->x << p2->y << p2->angle << p2->life; //Пакуем значения координат в Пакет
            socket2.send(packet2); //Отправка данных
            packet2.clear(); //Чистим пакет
        }
 
    if(mode == 'c'){//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    //Чистим пакет
    //управление персонажем
    if(Keyboard::isKeyPressed(Keyboard::Right)) p->angle += 3;
    if(Keyboard::isKeyPressed(Keyboard::Left)) p->angle -= 3;
    if(Keyboard::isKeyPressed(Keyboard::Up)) p->thrust = true;
    else p->thrust = false;
    if (event.key.code == Keyboard::P)
    {
        bullet *b = new bullet();
        b->settings(sBullet,p->x,p->y,p->angle,10);
        entities.push_back(b);
    }

    for(auto a:entities)
     for(auto b:entities)
    {
      if (a->name=="asteroid" && b->name=="bullet")
       if ( isCollide(a,b) )
           {
            a->life=false;
            b->life=false;

            Entity *e = new Entity();
            e->settings(sExplosion,a->x,a->y);
            e->name="explosion";
            entities.push_back(e);


            for(int i=0;i<2;i++)
            {
             if (a->R==15) continue;
             Entity *e = new asteroid();
             e->settings(sRock_small,a->x,a->y,rand()%360,15);
             entities.push_back(e);
            }

           }

      if (a->name=="player" && b->name=="asteroid")
       if ( isCollide(a,b) )
           {
            b->life=false;

            Entity *e = new Entity();
            e->settings(sExplosion_ship,a->x,a->y);
            e->name="explosion";
            entities.push_back(e);

            p->settings(sPlayer,W/2,H/2,0,20);
            p->dx=0; p->dy=0;
           }
    }


    if (p->thrust)  p->anim = sPlayer_go;
    else   p->anim = sPlayer;


    for(auto e:entities)
     if (e->name=="explosion")
      if (e->anim.isEnd()) e->life=0;

    if (rand()%2000000==0 && rand()%200200 == 0 )
     {
       asteroid *a = new asteroid();
       a->settings(sRock, 0,rand()%H, rand()%360, 25);
       entities.push_back(a);
     }

    for(auto i=entities.begin();i!=entities.end();)
    {
      Entity *e = *i;

      e->update();
      e->anim.update();

      if (e->life==false) {i=entities.erase(i); delete e;}
      else i++;
    }


    packet1 << p->x << p->y << p->angle << p->life; //Пакуем значения координат в Пакет
    socket1.send(packet1); //Отправка данных
    packet1.clear();
    socket2.receive(packet2); //Команда которая ожидает данных в виде пакета от клиентской части
    if(packet2 >> p2->x >> p2->y >> p2->angle >> p2->life){ //вытаскиваем значение из пакета в переменную x1 и у (действие уже происходит)
    cout << p2->x << ":" << p2->y << endl;}
    }


    app.draw(background);

    for(auto i:entities)
        i->draw(app);
    app.display();

    }//END
    return 0;
}