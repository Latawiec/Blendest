#include "public/App/App.hpp"

#include <UI/View/Main.hpp>
#include <Model/Main.hpp>

#include <Serializer/Serializer.hpp>

void App::Run()
{
    UI::View::Main mainView;
    Model::Main mainModel(mainView);

    Model::Serializer::Serializer::readConfigFile(mainModel);


    struct ConfigDropRAII {
        Model::Main& _model;

        ConfigDropRAII(Model::Main& model) : _model(model) {}
        ~ConfigDropRAII() {
            Model::Serializer::Serializer::writeConfigFile(_model);
        }
    } config(mainModel);

    mainView.Loop();
}