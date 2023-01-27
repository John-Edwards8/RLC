class Render {
private:
	//Render flag
	bool gRenderQuad = true;
public:
	Render(){}
	void render() {
		//Clear color buffer
		glClear( GL_COLOR_BUFFER_BIT );
		
		//Render quad
		if( this->gRenderQuad )
		{
			glBegin( GL_QUADS );
				glVertex2f( -0.5f, -0.5f );
				glVertex2f( 0.5f, -0.5f );
				glVertex2f( 0.5f, 0.5f );
				glVertex2f( -0.5f, 0.5f );
			glEnd();
		}
	}
	bool getRenderQuad() {
		return this->gRenderQuad;
	}
	void setRenderQuad(bool new_value) {
		this->gRenderQuad = new_value;
	}

};