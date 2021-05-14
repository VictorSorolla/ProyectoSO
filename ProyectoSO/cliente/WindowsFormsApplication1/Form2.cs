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
    public partial class Form2 : Form
    {

        public string user;
        public string pass1;
        public string pass2;
  
        public Form2()
        {
            InitializeComponent();
        }

        private void Form2_Load(object sender, EventArgs e)
        {
            
        }

        private void button1_Click(object sender, EventArgs e)
        {
            int res;
            res = string.Compare(password1.Text, password2.Text);

            if (res == 0)
            {
                this.user = username.Text;
                this.pass1 = password1.Text;
                this.pass2 = password2.Text;
                this.Close();
            }
            else
                MessageBox.Show("Las contraseñas no coinciden. Por favor vuelva a teclearlas y clique al botón 'Registrar'");
        }
    }
}
