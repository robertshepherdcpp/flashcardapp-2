#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

#include "imgui.h"
#include "imgui-SFML.h"

#include<string>
#include<random>
#include<fstream>

struct flashcard {
    flashcard(std::string a, std::string b) { question = a, answer = b; }

    std::string question;
    std::string answer;

    std::string current = question;

    auto flip() 
    {
        if (current == question) { current = answer; }
        else { current = question; }
    }
};

int main() {

    int index_flashcard{};
    bool test_window = false;
    int random{};
    bool hasFinishedTest = false;
    bool hasFinishedQuestion = false;
    int random_q_pos{};
    bool congrats_screen = false;
    bool sorry_screen = false;
    bool flashcard_ = true;

    sf::RenderWindow window(sf::VideoMode(800, 600), "ImGui + SFML Example");
    window.setFramerateLimit(60);

    // Setup ImGui with SFML
    ImGui::SFML::Init(window);

    sf::Clock deltaClock;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);

            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        // change this for reading from a file later on.
        std::vector<flashcard> flashcards{
            flashcard("What is the capital of England?", "London"),
            flashcard("What is the capital of Spain?", "Madrid"),
            flashcard("What is the capital of France?", "Paris"),
            flashcard("What is the capital of Germany?", "Berlin"),
            flashcard("What is the capital of Scotland?", "Edinburgh")
        };

        // getting the data from the file
        std::ifstream f("flashcards.txt");
        std::string line{};
        while (std::getline(f, line))
        {
            std::string q{};
            std::string a{};
            // so now we have a string that looks something like this: asdlfkjas;fj? || as;dlfkjsdlf;j
            for (int i = 0; i < line.size() - 1; i++)
            {
                if (line.substr(i, i + 1) == "||")
                {
                    q = line.substr(0, i);
                    a = line.substr(i + 3, line.size());
                    flashcards.push_back(flashcard(q, a));
                }
            }

        }

        // imgui code

        // --------------------------------- FLASHCARD MANAGER -----------------------------------------------------
        ImGui::Begin("Flashcard manager");
        ImGui::Text((std::string("Current flashcard: ") + std::to_string(index_flashcard)).c_str());
        ImGui::SameLine();
        if (ImGui::Button("Next"))
        {
            if (index_flashcard < (flashcards.size() - 1))
            {
                index_flashcard += 1;
            }
            else {
                index_flashcard = 0;
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Before"))
        {
            if (index_flashcard > 0)
            {
                index_flashcard -= 1;
            }
            else {
                index_flashcard = flashcards.size() - 1;
            }
        }
        ImGui::NewLine();
        if (ImGui::Button("Test"))
        {
            test_window = true;
        }
        ImGui::End();
        // --------------------------------- FLASHCARD MANAGER -----------------------------------------------------

        // --------------------------------- FLASHCARD -------------------------------------------------------------
        if (flashcard_)
        {
            ImGui::Begin("Flashcard");
            ImGui::Text(flashcards[index_flashcard].current.c_str());
            if (ImGui::Button("flip"))
                flashcards[index_flashcard].flip();
            ImGui::End();
        }
        // --------------------------------- FLASHCARD -------------------------------------------------------------

        if (test_window)
        {
            ImGui::Begin("Test");
            // get a random number in the flashcards.
            if (!hasFinishedTest)
            {
                std::random_device dev;
                std::mt19937 rng(dev());
                std::uniform_int_distribution<std::mt19937::result_type> dist6(0, flashcards.size() - 3);
                random = dist6(rng);
                hasFinishedTest = true;
            }

            // now we have to display the question.
            ImGui::Text(flashcards[random].question.c_str());

            // now we give a range of options. (we give 3 options) each question has a random position of where the answer is.
            if (!hasFinishedQuestion)
            {
                std::random_device dev;
                std::mt19937 rng(dev());
                std::uniform_int_distribution<std::mt19937::result_type> dist6(0, 2);
                random_q_pos = dist6(rng);
                hasFinishedQuestion = true;
            }

            std::string choice1 = flashcards[random].answer;
            std::string choice2 = flashcards[random].answer;
            std::string choice3 = flashcards[random].answer;
            if (random_q_pos != 0) choice1 = flashcards[random].answer.c_str();
            if (random_q_pos != 1) choice2 = flashcards[random + 1].answer.c_str();
            if (random_q_pos != 2) choice3 = flashcards[random + 2].answer.c_str();

            if (ImGui::Button(choice1.c_str())) { if (choice1 == flashcards[random].answer) { congrats_screen = true; } else { sorry_screen = true; } }
            if (ImGui::Button(choice2.c_str())) { if (choice2 == flashcards[random].answer) { congrats_screen = true; } else { sorry_screen = true; } }
            if (ImGui::Button(choice3.c_str())) { if (choice3 == flashcards[random].answer) { congrats_screen = true; } else { sorry_screen = true; } }

            ImGui::NewLine();

            if (ImGui::Button("Next"))
            {
                std::random_device dev_a;
                std::mt19937 rng_a(dev_a());
                std::uniform_int_distribution<std::mt19937::result_type> dist6_a(0, flashcards.size() - 3);
                random = dist6_a(rng_a);

                std::random_device dev;
                std::mt19937 rng(dev());
                std::uniform_int_distribution<std::mt19937::result_type> dist6(0, 2);
                random_q_pos = dist6(rng);

                congrats_screen = false;
                sorry_screen = false;
            }
            if (ImGui::Button("Exit"))
            {
                test_window = false;
            }
            ImGui::End();
        }

        if (congrats_screen)
        {
            ImGui::Begin("Congrats!");
            ImGui::Text("Well done you got it right!");
            ImGui::End();
        }

        if (sorry_screen)
        {
            ImGui::Begin("No");
            ImGui::Text("Thats not right. Try again next time.");
            ImGui::End();
        }

        // imgui code

        window.clear();
        ImGui::SFML::Render(window);
        window.display();
    }

    // Cleanup
    ImGui::SFML::Shutdown();
    return 0;
}