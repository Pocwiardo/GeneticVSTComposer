#ifndef GENETIC_MELODY_GENERATOR_HPP
#define GENETIC_MELODY_GENERATOR_HPP

#include "mingus.hpp"
#include <map>
#include <random>
#include <string>
#include <vector>

class GeneticMelodyGenerator {
public:
  GeneticMelodyGenerator(int mode, const std::string &scale,
                         const std::pair<int, int> &noteRange, float diversity,
                         float dynamics, float arousal, float pauseAmount,
                         float valence, float jazziness, float weirdness,
                         const std::pair<int, int> &meter = {4, 4},
                         float noteDuration = 0.5, int populationSize = 128,
                         int numGenerations = 100);

  void set_coefficients(const std::map<std::string, float> &mu_values = {},
                        const std::map<std::string, float> &sigma_values = {},
                        const std::map<std::string, int> &weights = {});

  // Method for crossing two individuals (parents)
  std::pair<std::vector<int>, std::vector<int>>
  crossover(const std::vector<int> &parent1, const std::vector<int> &parent2);

  // Method for tournament selection
  std::vector<int>
  tournament_selection(const std::vector<std::vector<int>> &population,
                       int tournament_size = 4);

  // Declaration of a fitness function, which will be needed for
  // tournament_selection
  float fitness(const std::vector<int> &individual,
                const std::vector<std::vector<int>> &population);
  float average_fitness(const std::vector<std::vector<int>> &population);
  std::pair<float, float>
  min_max_fitness(const std::vector<std::vector<int>> &population);
  void mutate(std::vector<int> &melody);
  std::vector<std::vector<int>>
  run(float measures = 1, const std::vector<int> &template_individual = {});
  void test(int measures = 1, const std::string file_name = "fitness.txt");

private:
  std::vector<int> NOTES;
  std::vector<int> scale_notes;
  std::pair<int, int> meter;
  int mode;
  float noteDuration;
  int populationSize;
  int numGenerations;
  float mutationRate;
  float crossoverRate;
  int expectedLength;
  int notesRange;
  float diversity;
  float dynamics;
  float arousal;
  float valence;
  float jazziness;
  float weirdness;
  float pauseAmount;

  std::pair<float, float> fitness_intervals(const std::vector<int> &melody);
  float fitness_directional_changes(const std::vector<int> &melody);
  float fitness_melodic_contour(const std::vector<int> &melody);
  float fitness_note_range(const std::vector<int> &melody);
  float fitness_average_pitch(const std::vector<int> &melody);
  float fitness_pause_proportion(const std::vector<int> &melody);
  std::pair<float, float>
  fitness_scale_and_chord(const std::vector<int> &melody);
  float fitness_pitch_variation(const std::vector<int> &melody);
  float fitness_odd_index_notes(const std::vector<int> &melody);
  float fitness_note_diversity(const std::vector<int> &melody);
  float fitness_diversity_intervals(const std::vector<int> &melody);
  float fitness_rhythm(const std::vector<int> &melody);
  std::pair<float, float>
  fitness_log_rhythmic_value(const std::vector<int> &melody);
  float proportion_of_long_notes(const std::vector<int> &melody);
  float fitness_average_intervals(const std::vector<int> &melody);
  float fitness_small_intervals(const std::vector<int> &melody);
  float fitness_repeated_short_notes(const std::vector<int> &melody);
  float
  calculate_similarity_penalty(const std::vector<int> &melody,
                               const std::vector<std::vector<int>> &population);

  // Coefficients for the genetic algorithm
  std::map<std::string, float> muValues;
  std::map<std::string, float> sigmaValues;
  std::map<std::string, int> weights;

  std::mt19937 rng;
  std::uniform_real_distribution<float> prob_dist;
  std::vector<std::vector<int>> generate_population(int note_amount);
  std::vector<std::vector<int>> generate_population_from_template(
      const std::vector<int> &template_individual);
  std::vector<std::vector<int>> generate_population_fixed(int note_amount);
};

#endif // GENETIC_MELODY_GENERATOR_HPP
