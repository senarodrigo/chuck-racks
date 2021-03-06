//
//  FileContainerManagerUI.cpp
//  ChuckRacks
//
//  Created by Rodrigo Sena on 4/22/15.
//
//

#include "FileContainerManagerUI.h"
#include "ConsoleGlobal.h"


FileContainerManagerUI::FileContainerManagerUI(FileContainerManagerModel* managerModel): tabView(TabbedButtonBar::Orientation::TabsAtTop), currentViewMode(AppViewMode::RackView),
consoleHeight(50)
{
    mManagerModel = managerModel;
    //console = new CodeEditorComponent(mManagerModel->consoleDocument, &cppTokeniser);
    addAndMakeVisible(console = new TextEditor("Console"));
    ConsoleGlobal::Instance()->setConsoleComponent(console.get());
    ConsoleGlobal::Instance()->updateText();
     //console.
}


FileContainerManagerUI::~FileContainerManagerUI(){
    for (int i=0; i<fileContainerUIs.size(); i++) {
        fileContainerUIs[i]->removeChangeListener(this);
    }
}

void FileContainerManagerUI::paint(Graphics& g){
    //g.fillAll(Colour(38, 40, 49));
    g.setColour(Colours::darkgrey);
    g.drawFittedText("Please add a ChucK Editor", getWidth() * 0.5, getHeight() * 0.5, 100, 50, juce::Justification::centred, 2);
}

void FileContainerManagerUI::resized(){
    
}

void FileContainerManagerUI::init(){
    
    
    
    switch (currentViewMode)
    {
        case AppViewMode::RackView :
            scrollableView.setBounds(0, 0, getLocalBounds().getWidth(),
                                     getLocalBounds().getHeight()-consoleHeight);
            mainView.setBounds(getLocalBounds());
            addAndMakeVisible(&scrollableView);
            scrollableView.setViewedComponent(&mainView);
            scrollableView.setScrollBarsShown(true, false);
            break;
            
        case AppViewMode::TabView :
            tabView.setBounds(getLocalBounds());
            addAndMakeVisible(tabView);
            break;
        default:
            break;
    }
    
    addAndMakeVisible(console);
    console->setBounds(0, getLocalBounds().getHeight() - consoleHeight,
                       getLocalBounds().getWidth(), consoleHeight);
    console->setReadOnly(true);
    console->setMultiLine(true);
    console->setCaretVisible(false);
    console->setScrollbarsShown(true);
    console->setScrollBarThickness(10);
    
    for (int i=0; i<mManagerModel->fileContainerModels.size(); i++){
        addNewFileContainerUI(mManagerModel->fileContainerModels[i]);
    }
}

void FileContainerManagerUI::addNewFileContainerUI(FileContainerModel* fileContainerModel){
    FileContainerUI* newFileContainerUI = new FileContainerUI(fileContainerModel);
    fileContainerUIs.add(newFileContainerUI);
    newFileContainerUI->addChangeListener(this);
    newFileContainerUI->setViewMode(currentViewMode);
    switch (currentViewMode) {
        case AppViewMode::RackView :
            mainView.addAndMakeVisible(newFileContainerUI);
            newFileContainerUI->init();
            updateFileContainerUILayout();
            break;
        case AppViewMode::TabView :
            tabView.addTab("Test", Colour(100, 106, 127), newFileContainerUI, false);
            newFileContainerUI->init();
            break;
        default:
            break;
    }
    

}

void FileContainerManagerUI::updateFileContainerUILayout(){
    // Update the vertical position of each of the file containers
    for(int i=0; i<fileContainerUIs.size(); i++)
    {
        FileContainerUI* currentFileContainerUI = fileContainerUIs[i];

        if (i==0)
        {
            currentFileContainerUI->setTopLeftPosition(0, 0);
        }else{
            currentFileContainerUI->setTopLeftPosition(0, fileContainerUIs[i-1]->getBottom()-1);
        }
    }
    
    if ( fileContainerUIs.size() > 0 )
    {
        mainView.setBounds(0, 0, getParentComponent()->getWidth(), fileContainerUIs.getLast()->getBottom());
    }
    
}

void FileContainerManagerUI::changeListenerCallback(ChangeBroadcaster *source){
    if(currentViewMode == AppViewMode::RackView)
    {
        // Attempt to cast the changebroadcaster as a FileContainerUI
        FileContainerUI* fileContainerUI = dynamic_cast<FileContainerUI*>(source);
        
        // If successful, update our layout
        if (fileContainerUI != nullptr ) {
            updateFileContainerUILayout();
        }
    }
}




