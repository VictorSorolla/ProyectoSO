using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Net;
using System.Net.Sockets;

namespace WindowsFormsApplication1
{
    public partial class Form3 : Form
    {
        public Socket server;
        public string mensaje;

        public Form3()
        {
            InitializeComponent();
        }

        private void Form3_Load(object sender, EventArgs e)
        {
            // Recibimos la respuesta del servidor
            string[] trozos = mensaje.Split('/');
            int codigo = Convert.ToInt32(trozos[0]);
            //mensaje = trozos[0].Split('\0')[0];
            //mensaje = trozos[1].Split('/')[0];
            //mensaje = trozos[1];


            matriz.ColumnCount = 2;
            matriz.RowCount = codigo;
            matriz.ColumnHeadersVisible = true;
            matriz.RowHeadersVisible = false;
            matriz.AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.Fill;
            matriz.Columns[0].HeaderText = " Usuario ";
            matriz.Columns[1].HeaderText = " Estado ";

            int i = 0;
            while (i < codigo)
            {
                string[] nombres = new string[10];
                nombres[i] = trozos[0];



                matriz.Rows[i].Cells[0].Value = nombres[i];
                matriz.Rows[i].Cells[1].Value = "Conectado";

                i++;
            }
        }

 

        private void label1_Click(object sender, EventArgs e)
        {
            Form4 f4 = new Form4();
            f4.ShowDialog();
        }
    }
}
