

class SimpleEmitter
{
    public:
        SimpleEmitter();
        deleteEmitter();

        void update(const float dt);

        void draw();

    private:
        struct Particle
        {
            vec3 position;
            float lifetime;
        };
        std::vector<Particle> particles;

        float positions[400];

        GLuint vertexBuffer;
        GLuint positionBuffer;
};

