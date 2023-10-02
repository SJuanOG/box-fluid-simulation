#include <iostream>
//Libreria para representación gráfica
#include <SFML/Graphics.hpp>

//Clase para definir propiedades de los campos
class Field
{
public:
    // Función para definir arreglo y dimensión
    void Dim(int,int); 

    //Definición para permitir perfecta comunicación entre clases
    friend class Solution;
    
    //Arreglo con paámetros del campo
    float *parameters;
    
    //Dimensiones del campo
    int x;
    int y;
    int size;

//Campos necesitados para la implementación
}u,v,dens,u_prev,v_prev,dens_prev,u_font,v_font,dens_font,diverge,p;

// Función para definir arreglo y dimensión
void Field::Dim(int xLeng, int yLeng)
{
    //Dimensiones del campo
    x = xLeng;
    y = yLeng;
    size = (x+2) * (y+2);
    
    //Re-definición del arreglo y llenado de ceros
    parameters = new float[(xLeng+2)*(yLeng+2)];
    for(int i=0;i<size-1;i++){
        parameters[i] = 0;
    }

}

//Clase para definir comportamiento del fluido
class Solution
{
public:
    
    //Definición de los valores ingresados por usuario
    Solution(int xSize, int ySize, float fludiff, float fluvisc, float dt);
    
    //Funciones necesarias para simulación gráfica
    void addSource(int,int,int,int);
    void update();
    void reset();
    void render(int, sf::Image &image);

    //Propiedades físicas, tasa difusión y viscosidad'
    float diff;
    float visc;
    
    //Parámetros de simulación
    float dt;
    int xLeng;
    int yLeng;

    //Funciones del comportamiento del fluido
    void diffuseV();
    void diffuseD();
    void advectV();
    void advectD();
    void projectV();
    //Necesidades matemáticas
    void set_bnd(int b, Field f);
    void swapPointers(float *&x0, float *&x1);
};

Solution::Solution(int xSize, int ySize, float fludiff, float fluvisc, float dt)
{
    //Definir paráemtros a utilizar en la implementación
    this->xLeng = xSize;
    this->yLeng = ySize;
    diff = fludiff;
    visc = fluvisc;
    this->dt = dt;
    //Dimensión de los arreglos de los campos
    u.Dim(xLeng, yLeng);
    v.Dim(xLeng, yLeng);
    dens.Dim(xLeng, yLeng);
    u_prev.Dim(xLeng, yLeng);
    v_prev.Dim(xLeng, yLeng);
    dens_prev.Dim(xLeng, yLeng);
    u_font.Dim(xLeng,yLeng);
    v_font.Dim(xLeng,yLeng);
    dens_font.Dim(xLeng,yLeng);
    diverge.Dim(xLeng, yLeng);
    p.Dim(xLeng, yLeng);
}

//Función que actualiza la simulación a cada paso de tiempo
void Solution::update()
{
    for (int i = 1; i <= xLeng-1; i++){
        for (int j = 1; j<=xLeng-1; j++){
            u_prev.parameters[i + j*(xLeng+2)] = u_font.parameters[i + j*(xLeng+2)];
            v_prev.parameters[i + j*(xLeng+2)] = v_font.parameters[i + j*(xLeng+2)];
            u_font.parameters[i + j*(xLeng+2)] = 0;
            v_font.parameters[i + j*(xLeng+2)] = 0;
        }
    }
    diffuseV();
    projectV();
    swapPointers(u_prev.parameters, u.parameters);
    swapPointers(v_prev.parameters, v.parameters);
    advectV();
    projectV();
    
    for (int i = 1; i <= xLeng-1; i++){
        for (int j = 1; j<=xLeng-1; j++){
            dens_prev.parameters[i + j*(xLeng+2)] += dens_font.parameters[i + j*(xLeng+2)];
            dens_font.parameters[i + j*(xLeng+2)] = 0;
        }
    }
    diffuseD();
    swapPointers(dens_prev.parameters, dens.parameters);
    advectD();
}

//Función que reinicia la simulación
void Solution::reset()
{
    for (int i = 1; i <= xLeng-1; i++){
        for (int j = 1; j<=xLeng-1; j++){
            u_prev.parameters[i + j*(xLeng+2)] = 0;
            v_prev.parameters[i + j*(xLeng+2)] = 0;
            u.parameters[i + j*(xLeng+2)] = 0;
            v.parameters[i + j*(xLeng+2)] = 0;
            dens_prev.parameters[i + j*(xLeng+2)] = 0;
            dens.parameters[i+j*(xLeng+2)] = 0;
        }
    }
}

//Función que dibuja pixel a pixel el resultado de la simulación
void Solution::render(int size, sf::Image &image)
{
    for (int i = 0; i <= xLeng-1+1; i++){
        for (int j = 0; j<=xLeng-1+1; j++){
            if (dens.parameters[i + j*(xLeng+2)] > 1){
                dens.parameters[i + j*(xLeng+2)] = 1;
            }
            for(int k = 0; k<=size-1; k++){
                for(int l = 0; l<=size-1;l++){
                    image.setPixel(i*size+k, j*size+l, sf::Color(255*dens.parameters[i+j*(xLeng+2)], 255*dens.parameters[i+j*(xLeng+2)],255*dens.parameters[i+j*(xLeng+2)]));
                }
            }
            dens_prev.parameters[i+j*(xLeng+2)] = dens.parameters[i+j*(xLeng+2)];
            u_prev.parameters[i+j*(xLeng+2)] = u.parameters[i+j*(xLeng+2)];
            v_prev.parameters[i+j*(xLeng+2)] = v.parameters[i+j*(xLeng+2)];
        }
    }
}

//Función que añade la interacción del usuario al cálculo
void Solution::addSource(int x0, int y0, int x1, int y1)
{
    int dx = x1 - x0;
    int dy = y1 - y0;
    if(abs(dx) >= abs(dy)){
        int yi = 1;
        if(dy<0){
            yi= -1;
            dy = -dy;
        }
        int D = 2*dy - dx;
        int y = y0;
        int elementID;
        if(dx>0){
            for (int i = x0; i <=x1-1; i++){
                elementID = i + (xLeng+2)*y;
                dens_font.parameters[elementID] = 1;
                u_font.parameters[elementID] = (x1 - x0)/dt;
                v_font.parameters[elementID] = (y1 - y0)/dt;
                if(D>0){
                    y += yi;
                    D -= 2*dx;
                }
                D += 2*dy;
            }
        }
        else if (dx<0){
            D = 2*dy+dx;
            for (int i = x0; i >=x1; i--){
                elementID = i + (xLeng+2)*y;
                dens_font.parameters[elementID] = 1;
                u_font.parameters[elementID] = (x1 - x0)/dt;
                v_font.parameters[elementID] = (y1 - y0)/dt;
                if(D>0){
                    y += yi;
                    D += 2*dx;
                }
                D += 2*dy;
            }
        }
    }
    else if (abs(dy) > abs(dx)){
        int xi = 1;
        if(dx<0){
            xi= -1;
            dx = -dx;
        }
        int D = 2*dx - dy;
        int x = x0;
        int elementID = 0;
        if(dy>0){
            for (int j = y0; j <=y1; j++){
                elementID = x + (xLeng+2)*j;
                dens_font.parameters[elementID] = 1;
                u_font.parameters[elementID] = (x1 - x0)/dt;
                v_font.parameters[elementID] = (y1 - y0)/dt;
                if(D>0){
                    x += xi;
                    D -= 2*dy;
                }
                D += 2*dx;
            }
        }
        else if (dy<0){
            D = 2*dx+dy;
            for (int j = y0; j >=y1; j--){
                elementID = x + (xLeng+2)*j;
                dens_font.parameters[elementID] = 1;
                u_font.parameters[elementID] = (x1 - x0)/dt;
                v_font.parameters[elementID] = (y1 - y0)/dt;
                if(D>0){
                    x += xi;
                    D += 2*dy;
                }
                D += 2*dx;
            }
        }
    }
}

//Función que simula la difusión de las componenetes de la velocidad como una interacción de estas con sus 4 celdas contiguas
void Solution::diffuseV()
{
    for (int i = 1; i<= xLeng; i++){
        for (int j=1; j<= yLeng; j++){
            u.parameters[ i + j * (xLeng + 2)] = u_prev.parameters[i + j * (xLeng + 2)] + visc*(0.25*u.parameters[ i -1 + j * (xLeng + 2)] + 0.25*u.parameters[ i +1 + j * (xLeng + 2)] + 0.25*u.parameters[ i + (j-1) * (xLeng + 2)] + 0.25*u.parameters[ i + (j+1) * (xLeng + 2)] - u_prev.parameters[i + j* (xLeng+2)]);
            v.parameters[ i + j * (xLeng + 2)] = v_prev.parameters[i + j * (xLeng + 2)] + visc*(0.25*v.parameters[ i -1 + j * (xLeng + 2)] + 0.25*v.parameters[ i +1 + j * (xLeng + 2)] + 0.25*v.parameters[ i + (j-1) * (xLeng + 2)] + 0.25*v.parameters[ i + (j+1) * (xLeng + 2)] - v_prev.parameters[i + j*(xLeng+2)]);
        }
    }
    set_bnd(1,u);
    set_bnd(2,v);
}

//Función que simula como el campo de velocidad se mueve consigo mismo con el uso de interpolación lineal
void Solution::advectV()
{
    for (int i = 1; i <= xLeng; i++){
        for(int j = 1; j <= xLeng; j++){
            double x,y,s1,s0,t1,t0;
            int i1,i0,j1,j0;
            float dt0 = dt*xLeng;
            x=y=s1=s0=t1=t0=0;
            i1=i0=j1=j0=0;
            x = i - dt0*u.parameters[i + (xLeng)*j]; 
            y = j - dt0*v.parameters[i + (xLeng)*j];
            
            if (x<0.5){
                x = 0.5;
            }
            if (x > xLeng +0.5){
                x = xLeng+0.5;
            }
            i0 = (int)x;
            i1 = i0+1;

            if (y<0.5){
                y = 0.5;
            }
            if (y > xLeng +0.5){
                y = xLeng+0.5;
            }
            j0 = (int)y;
            j1 = j0+1;
            
            s1 = x - i0;
            s0 = 1 - s1;
            t1 = y - j0;
            t0 = 1 - t1;
            
            u.parameters[i + (xLeng + 2)*j] = s0*(t0*u_prev.parameters[i0 + (xLeng + 2)*j0] + t1*u_prev.parameters[i0 + (xLeng + 2)*j1]) + s1*(t0*u_prev.parameters[i1 + (xLeng + 2)*j0] + t1*u_prev.parameters[i0 + (xLeng + 2)*j1]);
            v.parameters[i + (xLeng + 2)*j] = s0*(t0*v_prev.parameters[i0 + (xLeng + 2)*j0] + t1*v_prev.parameters[i0 + (xLeng + 2)*j1]) + s1*(t0*v_prev.parameters[i1 + (xLeng + 2)*j0] + t1*v_prev.parameters[i0 + (xLeng + 2)*j1]);
            
        }
    }
    set_bnd(1, u);
    set_bnd(2, v);
}

//Uso de la teoria de Hodge para asegurar la conservación de la masa
void Solution::projectV()
{
    float h = 1.0/xLeng;
    
    for (int i = 1; i <= xLeng; i++){
        for (int j = 1; j <= yLeng; j++){
            diverge.parameters[i + (xLeng+2)*j] = -0.5*h*(u.parameters[i+1+(xLeng+2)*j] - u.parameters[i-1+(xLeng+2)*j] + v.parameters[i + (xLeng+2)* (j+1)] - v.parameters[i + (xLeng+2)*(j-1)]);
            p.parameters[i + (xLeng+2)*j] = 0;
        }
    }
    set_bnd(1,u);
    set_bnd(2,v);
    
    for (int k = 0; k<5; k++){
        for (int i = 1; i <= xLeng; i++){
            for (int j = 1; j <= xLeng; j++){
                p.parameters[i + (xLeng+2)*j] = (diverge.parameters[i + (xLeng+2)*j] + p.parameters[i-1 + (xLeng+2)*j] + p.parameters[i+1 + (xLeng+2)*j] + p.parameters[i + (xLeng+2)*(j-1)] + p.parameters[i + (xLeng+2)*(j+1)])/4;
            }
        }
    }
    set_bnd(0,dens);
    
    for (int i = 1; i<=xLeng; i++){
        for (int j = 1; j<=xLeng; j++){
            u.parameters[i + (xLeng+2)*j] -= 0.5*(p.parameters[i+1 + (xLeng+2)*j] - p.parameters[i-1 + (xLeng+2)*j])/h;
            v.parameters[i + (xLeng+2)*j] -= 0.5*(p.parameters[i + (xLeng+2)*(j+1)] - p.parameters[i+ (xLeng+2)*(j-1)])/h;
        }
    }
    set_bnd(1,u);
    set_bnd(2,v);
}

//Función que simula la difusión de la densidad como una interacción de estas con sus 4 celdas contiguas
void Solution::diffuseD()
{

    for (int i = 1; i<= xLeng; i++){
        for (int j=1; j<= yLeng; j++){
            dens.parameters[ i + j * (xLeng + 2)] = dens_prev.parameters[i + j * (xLeng + 2)] + diff*(0.25*dens.parameters[ i -1 + j * (xLeng + 2)] + 0.25*dens.parameters[ i +1 + j * (xLeng + 2)] + 0.25*dens.parameters[ i + (j-1) * (xLeng + 2)] + 0.25*dens.parameters[ i + (j+1) * (xLeng + 2)] - dens_prev.parameters[i + j*(xLeng+2)]);
        }
    }
    set_bnd(0,dens);
}

//Función que simula como el campo de densidad se mueve a la par del campo de velocidades con el uso de interpolación lineal
void Solution::advectD()
{
    double x,y,s1,s0,t1,t0;
    int i1,i0,j1,j0;
    float dt0 = dt*xLeng;
    x=y=s1=s0=t1=t0 = 0;
    i1=i0=j1=j0=0;
    
    for (int i = 1; i <= xLeng; i++){
        for(int j = 1; j <= xLeng; j++){
            x = i - dt0*u.parameters[i + (xLeng+2)*j]; 
            y = j - dt0*v.parameters[i + (xLeng+2)*j];
            
            if (x<0.5){
                x = 0.5;
            }
            if (x > (xLeng+2) - 1.5){
                x = (xLeng+2) - 1.5;
            }
            i0 = (int)x;
            i1 = i0+1;

            if (y<0.5){
                y = 0.5;
            }
            if (y > (xLeng+2) - 1.5){
                y = (xLeng+2) - 1.5;
            }
            j0 = (int)y;
            j1 = j0+1;
            
            s1 = x - i0;
            s0 = 1 - s1;
            t1 = y - j0;
            t0 = 1 - t1;
            
            dens.parameters[i + (xLeng + 2)*j] = s0*(t0*dens_prev.parameters[i0 + (xLeng + 2)*j0] + t1*dens_prev.parameters[i0 + (xLeng + 2)*j1]) + s1*(t0*dens_prev.parameters[i1 + (xLeng + 2)*j0] + t1*dens_prev.parameters[i1 + (xLeng + 2)*j1]);
            
        }
    }
    set_bnd(0, dens);
}

//Función que controla las condiciones de forntera, no permitiendo que exista un flujo con las paredes
void Solution::set_bnd(int b, Field f)
{
    for (int i = 1; i <= xLeng; i++){
        f.parameters[i*(xLeng+2)] = (b == 1 ? -f.parameters[1 + i*(xLeng+2)] : f.parameters[1+ i*(xLeng+2)]);
        f.parameters[xLeng+1 + i*(xLeng+2)] = (b==1 ? -f.parameters[xLeng + i*(xLeng+2)] : f.parameters[xLeng + i*(xLeng+2)]);
        f.parameters[i] = (b==2 ? -f.parameters[i + 1*(xLeng+2)] : f.parameters[i + 1*(xLeng+2)]);
        f.parameters[ i + (xLeng+1)*(xLeng+2)] = (b==2 ? -f.parameters[i + (xLeng)*(xLeng+2)] : f.parameters[i + (xLeng)*(xLeng+2)]);
    }
    f.parameters[0] = 0.5*(f.parameters[1] + f.parameters[(xLeng+2)]);
    f.parameters[(xLeng+1)*(xLeng+2)] = 0.5*(f.parameters[1 + (xLeng+1)*(xLeng+2)] + f.parameters[(xLeng)*(xLeng+2)]);
    f.parameters[(xLeng+1)] = 0.5*(f.parameters[xLeng] + f.parameters[xLeng + 1 + 1]);
    f.parameters[(xLeng+1) + (xLeng+1)*(xLeng+2)] = 0.5*(f.parameters[xLeng + (xLeng+1)*(xLeng+2)] + f.parameters[(xLeng+1) + xLeng*(xLeng+2)]);
}

//Función para intercambiar punteros, útil en la redifinicion de arreglos
void Solution::swapPointers(float *&x0, float *&x1)
{
    float *memory = x0;
    x0 = x1;
    x1 = memory;
}


int main()
{
    //Parámtros de la caja
    int xLeng = 200;
    int yLeng = 200;
    int elementSize = 4;
    //Parámetros del fluido
    float diff = 0.07;
    float visc = 0.9975;
    
    //Parámetros de simulación
    int framerate = 60;
    float dt = 1.0/framerate;
    
    //Información del mouse
    int mouseX = 0;
    int mouseY = 0;
    int lastMouseX = 0;
    int lastMouseY = 0;
    
    //Configuración de la ventana de graficación
    sf::RenderWindow window(sf::VideoMode((xLeng+2)*elementSize,(yLeng+2)*elementSize), "Project");
    window.setFramerateLimit(framerate);
    
    //Herramientas de renderizado
    sf::Image image;
    sf::Texture texture;
    sf::Sprite sprite;
    image.create((xLeng+2)*elementSize, (yLeng+2)*elementSize, sf::Color::Black);
    
    //Fluido a simular
    Solution fluid(xLeng,yLeng,diff,visc,dt);
    
    while (window.isOpen()){
        //Detalles de la ventana
        sf::Event event;
        
        while (window.pollEvent(event)){
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
                window.close();
        }
        //Obtención de acción del usuario
        lastMouseX = mouseX;
        lastMouseY = mouseY;
        sf::Vector2i localPosition = sf::Mouse::getPosition(window);
        mouseX = localPosition.x;
        mouseY = localPosition.y;
        
        //Añadir acción del usuario
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)){
            if ((mouseX / elementSize > 0 && mouseX / elementSize < xLeng - 1 && mouseY/elementSize > 0 && mouseY/elementSize < yLeng - 1) && (lastMouseX / elementSize > 0 && lastMouseX / elementSize < xLeng - 1 && lastMouseY/elementSize > 0 && lastMouseY/elementSize < yLeng - 1)){
                fluid.addSource(lastMouseX/elementSize, lastMouseY/elementSize, mouseX/elementSize, mouseY/elementSize);
            }
        }
        if (sf::Mouse::isButtonPressed(sf::Mouse::Right)){
            fluid.reset();
        }

        //Simular paso a paso
        fluid.update();
        fluid.render(elementSize,image);
        texture.loadFromImage(image);
        sprite.setTexture(texture,true);
        window.draw(sprite);
        window.display();
    }
    return 0;
}
