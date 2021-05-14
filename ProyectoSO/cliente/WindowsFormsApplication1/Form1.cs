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
using System.Threading;


namespace WindowsFormsApplication1
{
    public partial class Form1 : Form
    {
        Form3 f3 = new Form3();
        Socket server;
        Thread atender;
        string message;
        string invitado;
        string texto;

        List<string> invitados = new List<string>();

        delegate void DelegadoParaEscribir(int code, string[] trozos);
        delegate void DelegadoParaChat(string[] trozos);

        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
           
           //InitializeComponent();
           //CheckForIllegalCrossThreadCalls = false; 
        }

        public void PonConectado(int code, string[] trozos)
        {
            matriz.ColumnCount = 1;
            matriz.RowCount = code;
            matriz.ColumnHeadersVisible = true;
            matriz.RowHeadersVisible = false;
            matriz.AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.Fill;
            matriz.Columns[0].HeaderText = " Conectados ";

            int i = 0;
            while (i < code)
            {
                string[] nombres = new string[10];
                nombres[i] = trozos[i + 1];
                matriz.Rows[i].Cells[0].Value = nombres[i];

                i++;
            }
            matriz.Refresh();
        }

        public void PonMensaje( string [] trozos)
        {
            //listBox1.Text = "---PANAS CHAT---";
            invitado = trozos[0];
            texto = trozos[1];
            texto = trozos[0] + " : " + texto;
            listBox1.Items.Add(texto);
        }

        private void AtenderServidor()
        {
            while (true)
            {
                
                byte[] msg2 = new byte[80];
                server.Receive(msg2);
                string mssg = Encoding.ASCII.GetString(msg2).Split('\0')[0];
                string [] trozos = mssg.Split('-');
                int codigo = Convert.ToInt32(trozos[0]);
                string mensaje = trozos[1];

                switch (codigo)
                {
                    case 1:
                        string[] piezas = mensaje.Split('/');
                        int code = Convert.ToInt32(piezas[0]);
                       
                        DelegadoParaEscribir delegado = new DelegadoParaEscribir(PonConectado);
                        matriz.Invoke(delegado, new object[] { code, piezas });
                    break;

                    case 2:
                            MessageBox.Show(mensaje);
                    break;
                  
                    case 3:
                            MessageBox.Show(mensaje);
                    break;
                    case 4:
                            DialogResult resultado;
                            resultado = MessageBox.Show(mensaje + " te ha enviado una invitación.", "Invitación", MessageBoxButtons.OKCancel);
                            if (resultado == DialogResult.OK)
                            {
                                string message = "5/" + mensaje + "/0";
                                // Enviamos al servidor el nombre tecleado
                                byte[] msg = System.Text.Encoding.ASCII.GetBytes(message);
                                server.Send(msg);
                            }
                            else
                            {
                                string message = "5/" + mensaje + "/1";
                                // Enviamos al servidor el nombre tecleado
                                byte[] msg = System.Text.Encoding.ASCII.GetBytes(message);
                                server.Send(msg);
                            }
                    break;
                    case 5:
                        string[] pieces = mensaje.Split('/');
                        int codigoAcepta = Convert.ToInt32(pieces[1]);
                        if (codigoAcepta == 0)
                        {
                            MessageBox.Show(pieces[0] + " ha ACEPTADO tu invitación");
                            invitado = pieces[0];
                        }
                        else
                            MessageBox.Show(pieces[0] + " ha RECHAZADO tu invitación");
                    break;
                    case 6:
                        pieces = mensaje.Split('/');
                        DelegadoParaChat delegadoChat = new DelegadoParaChat(PonMensaje);
                        listBox1.Invoke(delegadoChat, new object[] { pieces });
                    break;
                }
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            //Creamos un IPEndPoint con el ip del servidor y puerto del servidor 
            //al que deseamos conectarnos
            IPAddress direc = IPAddress.Parse("147.83.117.22");
            //192.168.56.101
            //147.83.117.22
            IPEndPoint ipep = new IPEndPoint(direc, 50068);
            

            //Creamos el socket 
            server = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            try
            {
                server.Connect(ipep);//Intentamos conectar el socket
                this.BackColor = Color.Green;
            }
            catch (SocketException ex)
            {
                //Si hay excepcion imprimimos error y salimos del programa con return 
                MessageBox.Show("No he podido conectar con el servidor");
                return;
            }

            //pongo en marcha el thread que atenderá los mensajes del servidor
            ThreadStart ts = delegate { AtenderServidor(); };
            atender = new Thread(ts);
            atender.Start();

        }


        private void button3_Click(object sender, EventArgs e)
        {
            //Mensaje de desconexión
            string mensaje = "0/";
        
            byte[] msg = System.Text.Encoding.ASCII.GetBytes(mensaje);
            server.Send(msg);

            // Nos desconectamos
            
            this.BackColor = Color.Gray;
            server.Shutdown(SocketShutdown.Both);
            server.Close();
            atender.Abort();

        }

        private void button5_Click(object sender, EventArgs e)
        {
            string mensaje = "2/" + Username.Text + "/" + Password.Text;
            // Enviamos al servidor el nombre tecleado
            byte[] msg = System.Text.Encoding.ASCII.GetBytes(mensaje);
            server.Send(msg);

        }


        private void button2_Click_1(object sender, EventArgs e)
        {
            Form2 f2 = new Form2();
            f2.ShowDialog();

            string mensaje = "3/" + f2.user + "/" + f2.pass1;
            // Enviamos al servidor el nombre tecleado
            byte[] msg = System.Text.Encoding.ASCII.GetBytes(mensaje);
            server.Send(msg);
            
        }

        private void button4_Click(object sender, EventArgs e)
        {
            string mensaje = "1/" + Username.Text + "/" + Password.Text;
            // Enviamos al servidor el nombre tecleado.
            byte[] msg = System.Text.Encoding.ASCII.GetBytes(mensaje);
            server.Send(msg);

        }

        private void invitarBTN_Click(object sender, EventArgs e)
        {
            for (int i = 0; i < invitados.Count; i++)
            {
                message = string.Concat(invitados[i]);
            }
            
            string mensaje = "4/" + message;
            byte[] msg = System.Text.Encoding.ASCII.GetBytes(mensaje);
            server.Send(msg);
        }

        private void matriz_CellClick(object sender, DataGridViewCellEventArgs e)
        {
            invitadosBox.AppendText(Convert.ToString(matriz.CurrentRow.Cells[0].Value) + Environment.NewLine);
            invitados.Add(Convert.ToString(matriz.CurrentRow.Cells[0].Value));
        }

        private void chatBTN_Click(object sender, EventArgs e)
        {
            string mensaje = "6/"+ invitado + "/" + chatBox.Text;
            byte[] msg = System.Text.Encoding.ASCII.GetBytes(mensaje);
            server.Send(msg);
        }
    }
}
