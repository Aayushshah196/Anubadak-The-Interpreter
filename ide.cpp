#include "ide.h"
#include <string>
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "visitor/interpreter.h"
#include "visitor/senamticAnalysis.h"
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

BEGIN_EVENT_TABLE(MainFrame, wxFrame)
EVT_MENU(NEWPROJECT, MainFrame::NewProject)
EVT_MENU(OPENPROJECT, MainFrame::OpenProject)
EVT_MENU(OPENFILE, MainFrame::OpenFile)
EVT_MENU(MENU_Close, MainFrame::CloseFile)
EVT_MENU(MENU_Save, MainFrame::SaveFile)
EVT_MENU(MENU_SaveAs, MainFrame::SaveFileAs)
EVT_MENU(MENU_Quit, MainFrame::Quit)

EVT_MENU(PROJECT_ADDCLASS, MainFrame::AddClass)
EVT_MENU(PROJECT_ADDFILE, MainFrame::AddFile)
EVT_MENU(PROJECT_DELETE, MainFrame::DelFile)

EVT_MENU(RUN, MainFrame::RunCode)
EVT_MENU(DEBUG, MainFrame::DebugCode)

EVT_TREE_SEL_CHANGED(FILETREE, MainFrame::OnTreeSelectionChanged)

END_EVENT_TABLE();


IMPLEMENT_APP(MainApp) // Initializes the MainApp class and tells our program
// to run it
bool MainApp::OnInit()
{
    MainFrame* MainWin = new MainFrame(wxT("Hello World!"), wxPoint(1, 1), wxSize(1500, 900)); // Create an instance of our frame, or window
    MainWin->Show(TRUE); // show the window
    SetTopWindow(MainWin);// and finally, set it as the main window

    return TRUE;
}

MainFrame::MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
    : wxFrame((wxFrame*)NULL, -1, title, pos, size)
{
    CreateStatusBar(2);
    MainMenu = new wxMenuBar();
    wxMenu* FileMenu = new wxMenu();
    wxMenu* Project = new wxMenu();
    wxMenu* RunMenu = new wxMenu();

    FileMenu->Append(NEWPROJECT, wxT("&New Project"),
        wxT("Create a new project"));
    FileMenu->Append(OPENPROJECT, wxT("&Open Project"),
        wxT("Open an existing project"));
    FileMenu->Append(OPENFILE, wxT("&Open File"),
        wxT("Open an existing file"));
    FileMenu->Append(MENU_Close, wxT("&Close"),
        wxT("Close the current document"));
    FileMenu->Append(MENU_Save, wxT("&Save"),
        wxT("Save the current document"));
    FileMenu->Append(MENU_SaveAs, wxT("&Save As"),
        wxT("Save the current document under a new file name"));
    FileMenu->Append(MENU_Quit, wxT("&Quit"),
        wxT("Quit the editor"));

    Project->Append(PROJECT_ADDFILE, wxT("&Add Class"),
        wxT("Create a new class"));
    Project->Append(PROJECT_ADDFILE, wxT("&Add File"),
        wxT("Adds new file to project"));
    Project->Append(PROJECT_DELETE, wxT("&Delete File"),
        wxT("Deletes the file"));

    RunMenu->Append(RUN, wxT("&Run Code"),
        wxT("Runs the code"));
    RunMenu->Append(DEBUG, wxT("&Debug Code"),
        wxT("Debugs the program"));

    MainMenu->Append(FileMenu, wxT("File"));
    MainMenu->Append(Project, wxT("Project"));
    MainMenu->Append(RunMenu, wxT("Build"));

    SetMenuBar(MainMenu);

    TextCtrl = new wxTextCtrl(this, TEXT_Main,
        wxT("Hi!"), wxDefaultPosition, wxDefaultSize,
        wxTE_MULTILINE | wxTE_RICH, wxDefaultValidator, wxTextCtrlNameStr);

    //wxStatusBar statusbar = new wxStatusBar(this, wxID_ANY, wxSTB_DEFAULT_STYLE,"Status Bar" );

    wxFont font = TextCtrl->GetFont();
     font.SetPointSize(font.GetPointSize() + 1);
    TextCtrl->SetFont(wxFont(13, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
        wxFONTWEIGHT_NORMAL, false));
    //Maximize();
   // Connect(FILETREE, wxEVT_TREE_SEL_CHANGED, (wxObjectEventFunction)&MainFrame::OnFilePressed);
}

void MainFrame::projectView(wxString& path)
{
    mainsizer = new wxBoxSizer(wxVERTICAL);
    splittermain = new wxSplitterWindow(this, wxID_ANY);
    mainsizer->Add(splittermain, 1, wxEXPAND, 5);

    fileTree = new wxPanel(splittermain,PANEL, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxNO_BORDER);
    group1 = new wxStaticBoxSizer(wxVERTICAL, fileTree, _T("project"));
    group1->SetMinSize(200,800);

    //fileTree = new wxPanel(this, PANEL, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxNO_BORDER);

    // Create a wxTreeCtrl control and add a few nodes to it

    /*imagelist = new wxImageList(16, 16);
    imagelist->Add(wxArtProvider::GetBitmap(wxART_FOLDER, wxART_OTHER, wxSize(16, 16)));
    imagelist->Add(wxArtProvider::GetBitmap(wxART_REPORT_VIEW, wxART_OTHER, wxSize(16, 16)));
    treeCtrl->AssignImageList(imagelist);*/

    treeCtrl = new wxTreeCtrl(fileTree, FILETREE, wxPoint(16, 48), wxSize(200, 800), wxTR_HAS_BUTTONS);
    wxDir my_dir;
    bool isopen = false;
    //treeCtrl->DeleteChildren(root);
    root = treeCtrl->AddRoot(path, 0);
    treeCtrl->SetItemText(root, path);
    isopen = my_dir.Open(path);

    wxString filename;
    bool cont = false;
    bool con = false;

    if (isopen)  con = my_dir.GetFirst(&filename, wxEmptyString, wxDIR_DIRS);
    while (con and isopen)
    {
        treeCtrl->AppendItem(root, filename, 0);
        con = my_dir.GetNext(&filename);
    }

    if (isopen)  cont = my_dir.GetFirst(&filename, wxEmptyString, wxDIR_FILES);
    while (cont and isopen)
    {
        treeCtrl->AppendItem(root, filename, 1);
        cont = my_dir.GetNext(&filename);
    }
    treeCtrl->ExpandAll();

    /*TextCtrl = new wxTextCtrl(fileTree, EDITIOR,
        wxT("ma ta TextCTrl ho"), wxDefaultPosition, wxDefaultSize,
        wxTE_MULTILINE | wxTE_RICH, wxDefaultValidator, wxTextCtrlNameStr);

    // Set up the sizer for the panel
   /* wxBoxSizer* panelSizer = new wxBoxSizer(wxHORIZONTAL);
    panelSizer->Add(treeCtrl, 1, wxEXPAND);
    panelSizer->Add(TextCtrl, 1, wxEXPAND);
    fileTree->SetSizer(panelSizer);*/

    // Set up the sizer for the frame and resize the frame
    // according to its contents
   /* wxBoxSizer* topSizer = new wxBoxSizer(wxHORIZONTAL);
    topSizer->Add(fileTree, 1, wxEXPAND);
    SetSizerAndFit(topSizer);*/

    group1->Add(treeCtrl, 1, wxALL | wxEXPAND, 5);
    fileTree->SetSizer(group1);

    editor = new wxPanel(splittermain, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxNO_BORDER);
    group2 = new wxStaticBoxSizer(wxVERTICAL, editor, _T("Text Editor"));
    group2->SetMinSize(700,800);
    delete TextCtrl;
    TextCtrl = new wxTextCtrl(editor, EDITIOR,
        wxT(""), wxDefaultPosition, wxDefaultSize,
        wxTE_MULTILINE);
    TextCtrl->SetFont(wxFont(13, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
        wxFONTWEIGHT_NORMAL, false));
    group2->Add(TextCtrl, 1, wxALL | wxEXPAND, 5);
    editor->SetSizer(group2);

    splittermain->SetSashGravity(0.1);
    splittermain->SplitVertically(fileTree, editor);

    SetSizer(mainsizer);
    mainsizer->SetSizeHints(this);

    Maximize();
}

void MainFrame::NewProject(wxCommandEvent& WXUNUSED(event))
{  
    wxFileDialog* OpenDialog = new wxFileDialog(
        this, _("New Folder"), wxEmptyString, wxEmptyString,
        _("*.*"), wxFD_OPEN, wxDefaultPosition);

    // Creates a "open file" dialog with 4 file types
    if (OpenDialog->ShowModal() == wxID_OK) // if the user click "Open" instead of "cancel"
    {
        path = OpenDialog->GetPath();
        // Sets our current document to the file the user selected
        wxMkDir(path);
    }
    projectView(path);
}

void MainFrame::OpenProject(wxCommandEvent& WXUNUSED(event))
{
    wxDir my_dir;
    dirdlg = new wxDirDialog(this, _("Select directory"), wxEmptyString, wxDD_DEFAULT_STYLE, wxDefaultPosition, wxDefaultSize, _T("wxDirDialog"));
    dirdlg->SetPath("C:\\");
    int dir = dirdlg->ShowModal();
    if (dir == wxID_OK)
    {
        path = dirdlg->GetPath();
        projectView(path);
    }
    delete dirdlg;
}

void MainFrame::OpenFile(wxCommandEvent& WXUNUSED(event))
{
}

void MainFrame::CloseFile(wxCommandEvent& WXUNUSED(event))
{
    TextCtrl->Clear();
}

void MainFrame::SaveFile(wxCommandEvent& WXUNUSED(event))
{
    TextCtrl->SaveFile(wxT("hello.txt"));
}

void MainFrame::SaveFileAs(wxCommandEvent& WXUNUSED(event))
{
    wxFileDialog* SaveDialog = new wxFileDialog(
        this, _("Save File As"), wxEmptyString, wxEmptyString,
        _("*.*"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT, wxDefaultPosition);

    // Creates a Save Dialog with 4 file types
    if (SaveDialog->ShowModal() == wxID_OK) // If the user clicked "OK"
    {
        wxString CurrentDocPath = SaveDialog->GetPath();
        // set the path of our current document to the file the user chose to save under
        TextCtrl->SaveFile(CurrentDocPath); // Save the file to the selected path
    }

    // Clean up after ourselves
    SaveDialog->Destroy();
}

void MainFrame::Quit(wxCommandEvent& WXUNUSED(event))
{
    Close(TRUE); // Tells the OS to quit running this process
}

//function for running the code of interpreter
void MainFrame::RunCode(wxCommandEvent& WXUNUSED(event))
{
    //Display console for output
    wxFrame* console = new wxFrame((wxFrame*)NULL, -1, "Output", wxPoint(100, 100), wxSize(700, 600));
    console->Show(true);
    wxTextCtrl* output = new wxTextCtrl(console, CONSOLE,
        "", wxDefaultPosition, console->GetSize(),
        wxTE_MULTILINE | wxTE_RICH, wxDefaultValidator, wxTextCtrlNameStr);
    output->SetBackgroundColour(*wxBLACK);
    output->SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
        wxFONTWEIGHT_BOLD, false));
    output->SetForegroundColour(*wxWHITE);

    wxString text = TextCtrl->GetValue();
    std::string program(text.begin(), text.end());
    text = "";
    lexer::Lexer lexer(program,output);
    std::cout << "LENGTH OF TOKENS=" << lexer.tokens.size() << std::endl;
    for (size_t i = 0; i < lexer.tokens.size(); i++)
    {
        output->AppendText("token = " + lexer.tokens[i].getVal() + " \t\t\t\t and   type =" + std::to_string(lexer.tokens[i].getType()));
        std::cout << "token = " << lexer.tokens[i].getVal() << " \t\t\t\t and   type =" << lexer.tokens[i].getType() << std::endl;

    }
    parser::ASTProgramNode* prog = nullptr;
    parser::Parser parser(&lexer,output);
    try {

        prog = parser.parse_program();

    }
    catch (std::exception & exception)
    {
        std::cerr << "error :" << exception.what() << std::endl;
    }
    try {
        SemanticAnalyser semantic(output);
        semantic.visit(prog);
    }
    catch (std::exception & e) {
        std::cerr << "ERROR:: semantic analysisis error occured  description:: " << e.what() << std::endl;
    }

    try {
        Interpreter interpreter(output);
        interpreter.visit(prog);
    }
    catch (std::exception & e) {
        std::cerr << "interprete ERROR:" << e.what() << std::endl;
    }
    output->AppendText("\nPress any key to continue...");
}

void MainFrame::DebugCode(wxCommandEvent& WXUNUSED(event))
{
}

void MainFrame::AddClass(wxCommandEvent& WXUNUSED(event))
{
}

void MainFrame::AddFile(wxCommandEvent& WXUNUSED(event))
{
    wxFileDialog* OpenDialog = new wxFileDialog(
        this, _("Choose a file to open"), wxEmptyString, wxEmptyString,
        _("*.*"), wxFD_OPEN, wxDefaultPosition);

    // Creates a "open file" dialog with 4 file types
    if (OpenDialog->ShowModal() == wxID_OK) // if the user click "Open" instead of "cancel"
    {
        path = OpenDialog->GetPath();

        // Sets our current document to the file the user selected
        projectView(path);
        TextCtrl->LoadFile(path); //Opens that file
    }
}

void MainFrame::DelFile(wxCommandEvent& WXUNUSED(event))
{
}

void MainFrame::OnTreeSelectionChanged(wxTreeEvent& event)
{
    wxTreeItemId selectedNode = event.GetItem();
    wxString nodeText = path +"\\"+ treeCtrl->GetItemText(selectedNode);
    TextCtrl->LoadFile(nodeText);
}
