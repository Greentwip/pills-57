import System.Collections.Generic
import System.Linq
import System.Text
import System.Threading.Tasks
import Microsoft.Xna.Framework
import Microsoft.Xna.Framework.Graphics
import Microsoft.Xna.Framework.Input

public class Pills : Game {
    private var _graphics: GraphicsDeviceManager;
    private var _spriteBatch: SpriteBatch?;

    public init(){
        _graphics = GraphicsDeviceManager(self);

        Content.RootDirectory = "Content";
        IsMouseVisible = true;
    }

    internal override func Initialize()
    {
        // TODO: Add your initialization logic here

        super.Initialize();
    }

    internal override func LoadContent()
    {
        _spriteBatch = SpriteBatch(GraphicsDevice);

        // TODO: use this.Content to load your game content here
    }

    internal override func Update(_ gameTime: GameTime)
    {
        // TODO: Add your update logic here

        super.Update(gameTime);
    }

    internal override func Draw(_ gameTime: GameTime)
    {
        GraphicsDevice.Clear(Color.CornflowerBlue);

        // TODO: Add your drawing code here

        super.Draw(gameTime);
    }

}