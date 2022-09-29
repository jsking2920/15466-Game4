#include "Mode.hpp"

#include "Scene.hpp"
#include "Sound.hpp"
#include "TextRenderer.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>

struct PlayMode : Mode {
	PlayMode();
	virtual ~PlayMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	//----- game state -----

	//input tracking:
	struct Button {
		uint8_t downs = 0;
	} enter, space, lmb, one, two;

	//local copy of the game scene (so code can change it during gameplay):
	Scene scene;

	std::shared_ptr< Sound::PlayingSample > cur_loop;
	
	//camera
	Scene::Camera *camera = nullptr;

	TextRenderer *display_text = nullptr;
	TextRenderer *body_text = nullptr;
	uint8_t display_font_size = 128;
	uint8_t body_font_size = 25;

	float timer = 0;
	float tick_rate = 0.1f;
	bool waiting = true;
	bool finished_poem = false;

	int16_t cur_line = -1; // start off with input prompt (line -1)
	uint16_t cur_index = 0;
	std::string cur_string = "";

	float line_spacing = 5.0f;
	float x_anchor = 0.28f; // left aligned; relative to width 
	float y_anchor = 0.25f; // bottom aligned; relative to height
	
	struct Poem {
		std::string title;
		std::string verb;
		std::vector<std::string> lines;
		Poem *left = nullptr;
		Poem *right = nullptr;
	} wildflowers, prayers, hudson;

	Poem cur_poem;

	void SetupPoems() {
		wildflowers.title = "The Wildflower Reserve";
		wildflowers.verb = "Languish";
		wildflowers.lines = {
				"Somewhere on those narrow dirt footpaths that wind through the Appalachian hills",
				"my foot met a gnarled root, or maybe a rock,",
				"and I tripped, falling forward into the back of the girl walking a step or two in front of me,",
				". . .",
				"a lilac blooming perennial",
				"shook by a brash",
				"spring wind.",
				". . .",
				"She turned back and giggled at my clumsiness and we smiled like children,",
				"so unburdened that we might have been hovering ever so slightly,",
				"and I slipped my hand into hers and we walked on,",
				". . .",
				"the Monongahela",
				"and the Allegheny",
				"becoming the Ohio.",
				". . .",
				"The day became night and day and night again and we treaded on,",
				"around bends and up hills, through rain and through snow, until her eyes,",
				"once lit up like fireflies searching for love on a warm summer night,",
				"dimmed and closed as eyes must always do, and I finally stopped to rest,",
				"laying my head upon her cold stone pillow in a field",
				". . .",
				"and I am lying there still,",
				"festering sweetly beneath the sun",
				"with a smile."
		};

		prayers.title = "Prayers";
		prayers.verb = "Regret";
		prayers.lines = {
				"The torpedo heater that sat",
				"on a couple of cinder blocks",
				"in the corner of the garage",
				"breathed fire, putting",
				"the fear of God in",
				"my grass-stained jeans",
				"every time I’d huddle up to it,",
				"praying that it might",
				"thaw more than my ankles.",
				"        My breath rose in front of my eyes",
				"        to join the smoke from his Black and Milds.",
				". . .",
				"I stepped over and around",
				"car parts I couldn’t name littering",
				"the cracked cement floor",
				"to get a wrench, 3/16ths,",
				"from the rusted-out toolbox",
				"that could have been my father.",
				"But it wasn’t because he",
				"was laying beneath a ’98 Chevy s10",
				"on the old, flattened box from",
				"the torpedo heater that he kept",
				"for lying on, as though it",
				"provided any cushion on the cement.",
				"        I put the wrench in the hand that reached out from beneath the truck,",
				"        all leather and rust, scars and burns. Short shorn nails, a tungsten carbide ring.",
				". . .",
				"Hymns from K105 were",
				"drowned out by the air",
				"compressor’s sermon and ",
				"the smell of motor oil,",
				"or him - I never figured out ",
				"which was which. He ",
				"ripped that v4 out while I",
				"read verses from the",
				"Haynes manual and ",
				"organized bolts in little",
				"magnetic dishes. Hollow,",
				"gutted, it stayed propped",
				"up on jack stands and looked",
				"at its own heart, hanging",
				"from a cherry picker.",
				"        I went inside to play games with mom,",
				"        he stayed in the garage, praying for enough duct tape and WD-40"
		};

		hudson.title = "In a Park by the Hudson";
		hudson.verb = "Bask";
		hudson.lines = {
				"The umbrella unused",
				"Beneath the sun."
		};

		wildflowers.left = &prayers;
		wildflowers.right = &hudson;
		hudson.left = &wildflowers;
		hudson.right = &prayers;
		prayers.right = &wildflowers;
		prayers.left = &hudson;

		cur_poem = wildflowers;
	}

	void SetMusic();
};
