/*
    Eric <itseric@mail.ru>
*/
#include <GL/glut.h>
#include <windows.h>
#include <cstdlib>
#include <iostream>
#include <time.h>
#include <vector>
#include <math.h>
#include <string.h>
#include <sstream>
#define QUOCIENTE_DIFICULDADE 30;
using namespace std;

int placar = 0, totalBombas = 0, proximaBomba = 0;
float posx = 0;
vector<pair <float,float> > posicaoBombas;
int passosTimer = 0, vidas = 3;
float porcentagemVermelho = 0.3;

void inicializar(void)
{
    glClearColor(0, 0, 0, 1);
}

void teclado(int tecla, int x, int y)
{
    if(tecla == GLUT_KEY_LEFT && posx > -0.9)
    {
        posx-=0.15;
        glutPostRedisplay();
    }
    else if(tecla == GLUT_KEY_RIGHT && posx < 0.9)
    {
        posx+=0.15;
        glutPostRedisplay();
    }

}

// Do tutorial da profa. Isabel Manssour
// em http://www.inf.pucrs.br/~manssour/OpenGL/Menus-Fonte.html
void desenhaTexto(char const *string)
{
    glPushMatrix();
    glColor3f(1, 0, 0.5);
    // Posição no universo onde o texto será colocado
    glRasterPos2f(-0.1, 0.9);
    // Exibe caracter a caracter
    while(*string)
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *string++);
    glPopMatrix();
}

void capturarBombas(void)
{
    vector<int> bombasCapturadas;
    int ondeEstou = 0;
    for(vector<pair <float,float> >::iterator it = posicaoBombas.begin(); it != posicaoBombas.end(); ++it, ondeEstou++)
    {
        double distancia = (double) fabs(fabs(posx) - fabs(it->first));
        if(distancia <= 0.15 && it->second <= -0.8)
        {
            bombasCapturadas.push_back(ondeEstou);
            cout << "Bomba " << ondeEstou << " com distancia " << distancia << " será eliminada!" << endl;
            placar++;
        }
    }
    for(vector<int>::iterator it = bombasCapturadas.begin(); it != bombasCapturadas.end(); ++it)
    {
        posicaoBombas.erase(posicaoBombas.begin() + *it);
    }
}


void gerarBombas(int valor)
{
    float posicaoAleatoriaX;

    // A cada 5 passos (500 ms) desenha bombas
    if(passosTimer == 4)
    {
        // No máximo 10 bombas na tela por vez
        if(totalBombas < 10)
        {
            if(placar == 0)
            {
                totalBombas++;
            }
            else
            {
                totalBombas += placar/QUOCIENTE_DIFICULDADE;
            }
            cout << "Placar atual : " << placar << endl;
            cout << "Total de bombas: " << totalBombas << endl;
        }

        // Cria novas bombas se necessário
        int bombasADesenhar = totalBombas - posicaoBombas.size();
        while(bombasADesenhar > 0)
        {
            srand(placar + totalBombas);
            posicaoAleatoriaX = (-1) + 0.1 * (rand() % 20 + 1);
            posicaoBombas.push_back(pair< float, float >(posicaoAleatoriaX, 0.4));
            bombasADesenhar--;
        }
        passosTimer = 0;
    }
    else{
        passosTimer++;
    }

    // Deleta imediatamente as bombas capturadas, se houver
    capturarBombas();

    // Atualiza a posiçao das bombas para que desçam a tela
    for(vector<pair <float,float> >::iterator it = posicaoBombas.begin(); it != posicaoBombas.end(); ++it)
    {
        if(it->second >= -1)
        {
            it->second -= 0.1;
        }
        else
        {
            if(vidas == 0){
                 exit(1);
            }
            else{
                vidas--;
                porcentagemVermelho+= 0.2;
            }
        }
    }

    glutPostRedisplay();
    glutTimerFunc(100, gerarBombas, 0);
}


void tela()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW); //ativa a operação na matriz model-view
    glLoadIdentity();           //reseta a matriz model-view

    glBegin(GL_QUADS);
    glColor3f(porcentagemVermelho, 0.5, 0.2);
    glVertex2f(1, 0.8);
    glVertex2f(-1, 0.8);
    glVertex2f(-1, -1);
    glVertex2f(1, -1);
    glEnd();

    glPushMatrix();
    glTranslatef(posx, 0, 0);

    glBegin(GL_QUADS);
    glColor3f(0, 0.5, 0.7);
    glVertex2f(0.1, -0.9);
    glVertex2f(-0.1, -0.9);
    glVertex2f(-0.1, -1);
    glVertex2f(0.1, -1);
    glEnd();

    glPopMatrix();

    for(vector<pair <float,float> >::iterator it = posicaoBombas.begin(); it != posicaoBombas.end(); ++it)
    {
        glPushMatrix();
        // Bombas serão pintadas de preto
        glColor3f(0, 0, 0);
        glTranslatef(it->first, it->second, 0);
        GLUquadricObj *qobj = gluNewQuadric();
        gluQuadricDrawStyle(qobj, GLU_FILL);
        gluSphere(qobj, 0.05, 30, 30);
        glPopMatrix();
    }

    stringstream ss;
    ss << placar;
    string textoPlacar = ss.str();
    desenhaTexto(textoPlacar.c_str());

    glutSwapBuffers();
}


void redesenha(GLsizei largura, GLsizei altura)
{

    if (altura == 0) altura = 1;
    GLfloat razao = (GLfloat)largura / (GLfloat)altura;

    glViewport(0, 0, largura, altura);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (largura >= altura)
    {
        gluOrtho2D(-1.0 * razao, 1.0 * razao, -1.0, 1.0);
    }
    else
    {
        gluOrtho2D(-1.0, 1.0, -1.0 / razao, 1.0 / razao);
    }
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(480, 480);
    glutInitWindowPosition(50, 50);
    glutCreateWindow("KABOOM");
    glutDisplayFunc(tela);
    glutReshapeFunc(redesenha);
    glutSpecialFunc(teclado);
    glutTimerFunc(100, gerarBombas, 0);
    inicializar();
    glutMainLoop();
    return 0;
}
