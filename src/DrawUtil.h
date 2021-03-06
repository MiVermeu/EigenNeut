#ifndef DRAWUTIL_H__
#define DRAWUTIL_H__

#define GL_SILENCE_DEPRECATION

#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <Eigen/Dense>

namespace DrawUtil{

void Line(sf::RenderWindow& window, const sf::Vector2f& a, const sf::Vector2f& b, const double width);
sf::Vector2f normalized(const sf::Vector2f a);
sf::Vector2f normal(const sf::Vector2f& a, const sf::Vector2f& b);
sf::Vector2f miter(const sf::Vector2f& a, const sf::Vector2f& b, const sf::Vector2f& c);
// Function to convert point-to-point vertex arrays into triangle strips.
std::vector<sf::Vertex> TriStrip(const std::vector<sf::Vertex>& drawing, const double thickness);

class TernaryGraph {
  private:
  // Graph parameters
  sf::CircleShape triangle; // Triangle is circle with three points lol.
  sf::Vector2f tcentre;
  double triangleR;
  double sideL;
  sf::Vector2f top;
  sf::Vector2f left;
  sf::Vector2f right;
  const int numDiv = 10;

  // Window and drawing parameters.
  sf::RenderWindow& window;
  double width;
  double height;
  sf::Vector2i pos;
  sf::Vector2f centre;
  sf::Vector2u oldWindowSize; // For scale when resizing.

  // Transformations.
  sf::Transform rot120;
  sf::Transform rot240;

  // Drawings.
  std::vector<std::vector<Eigen::Vector3d>> probs;
  std::vector<std::vector<sf::Vertex>> drawings;
  std::vector<std::vector<sf::Vertex>> highlights;

  // Textures aand sprites for labels
  sf::Texture nulabeltex[6];
  sf::Sprite nulabelsprite[6];
  bool anti = false; // Draw (anti)neutrino textures.

  public:
  // Animation time.
  double t = 0;

  TernaryGraph(sf::RenderWindow& window);

  // Function to transform a 3D vector into a 2D location on the ternary plot.
  sf::Vector2f TriPoint(float e, float mu, float tau);
  sf::Vector2f TriPoint(sf::Vector3f a) { return TriPoint(a.x,a.y,a.z); }
  sf::Vector2f TriPoint(Eigen::Vector3d a) { return TriPoint(a(0),a(1),(2)); }
  // Function to transform from 2D ternary point to 3D probability vector.
  sf::Vector3f InvTriPoint(const sf::Vertex& a);

  // Set position (top left corner) and size (wxh) of ternary plot.
  void setPosition(const double x, const double y) {
    pos.x = x; pos.y = y;
  }
  void setSize(const double w, const double h) {
    width = w; height = h;
  }
  
  // Set antineutrino labels.
  void setAnti(bool val) { anti = val; }

  // Draw everything in class.
  void draw();
  // Add a drawing in the form of a vector of 3D positions.
  void addDrawing(const std::vector<Eigen::Vector3d>& vec);
  // Clear all drawings.
  void clear() { probs.resize(0); }
  // Update all relevant parameters in case of a window size change.
  void updateWindow();

}; // class TernaryGraph

}

#endif