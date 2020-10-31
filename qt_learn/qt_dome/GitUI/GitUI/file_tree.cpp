#include "file_tree.h"

file_tree::file_tree()
{
}


QFileSystemModel * file_tree::drzewiak_plikow(){
    QFileSystemModel * model = new QFileSystemModel;
    model->setRootPath(QDir::currentPath());

    this->ui->drzewiak->setModel(model);
    this->ui->drzewiak->setRootIndex(model->index(QDir::rootPath()));
    this->ui->drzewiak->setColumnHidden( 1, true ); //nie wiem jak to działa, ale dzieki temu widze wszystkie pliki w przegladarce :D
    this->ui->drzewiak->setColumnHidden( 2, true );
    this->ui->drzewiak->setColumnHidden( 3, true );
    connect(this->ui->drzewiak,SIGNAL(activated(QModelIndex)),this,SLOT(otworz_dana_sciezke()));
}
