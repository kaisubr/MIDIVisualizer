#include <stdio.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>

#include "../helpers/ProgramUtilities.h"
#include "../helpers/MeshUtilities.h"
#include "../helpers/ResourcesManager.h"

#include "MIDIScene.h"

#ifdef _WIN32
#undef MIN
#undef MAX
#endif

MIDIScene::~MIDIScene(){}

MIDIScene::MIDIScene(const std::string & midiFilePath, float prerollTime): _midi(Midi::ReadFromFile(midiFilePath)) {
	// MIDI processing.
	_midi.Reset(prerollTime * 1000000, 0);
	_notes = _midi.Notes();
	
	// Load geometry and notes shared data.
	std::vector<float> vertices = {-0.5,-0.5, 0.5, -0.5, 0.5,0.5, -0.5, 0.5};
	std::vector<unsigned int> indices = {0, 1, 3, 3, 1, 2};
	_primitiveCount = indices.size();
	
	std::vector<float> data;
	for (auto& note : _notes) {
		if (note.note_id >= 21 && note.note_id <= 108) {
			data.push_back(float(note.note_id));
			data.push_back(note.start / 1000000.0f);
			data.push_back((note.end - note.start) / 1000000.0f);
			data.push_back((float(note.track_id)));
		}
	}
	_duration = _midi.GetSongLengthInMicroseconds() / 1000000.0f;
	_notesCount = _midi.AggregateNoteCount();
	std::cout << "[INFO]: Final track duration " << _duration << " sec." << std::endl;
	
	// Create an array buffer to host the geometry data.
	GLuint vbo = 0;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// Upload the data to the Array buffer.
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size() * 2, &(vertices[0]), GL_STATIC_DRAW);
	
	// Notes buffer.
	GLuint dataBufferId0 = 0;
	glGenBuffers(1, &dataBufferId0);
	glBindBuffer(GL_ARRAY_BUFFER, dataBufferId0);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * data.size(), &(data[0]), GL_STATIC_DRAW);
	
	// Enabled notes buffer (empty for now).
	_flagsBufferId = 0;
	glGenBuffers(1, &_flagsBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, _flagsBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 88, NULL, GL_DYNAMIC_DRAW);
	
	_uboKeyboard = 0;
	glGenBuffers(1, &_uboKeyboard);
	glBindBuffer(GL_UNIFORM_BUFFER, _uboKeyboard);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(float)*88, NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	
	// Programs.
	
	// Notes shaders.
	_programId = createGLProgramFromStrings(ResourcesManager::getStringForShader("notes_vert"), ResourcesManager::getStringForShader("notes_frag"));
	
	// Generate a vertex array (useful when we add other attributes to the geometry).
	_vao = 0;
	glGenVertexArrays (1, &_vao);
	glBindVertexArray(_vao);
	// The first attribute will be the vertices positions.
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glVertexAttribDivisor(0, 0);
	
	// The second attribute will be the notes data.
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, dataBufferId0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glVertexAttribDivisor(1, 1);
	
	// We load the indices data
	glGenBuffers(1, &_ebo);
 	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
 	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &(indices[0]), GL_STATIC_DRAW);

	glBindVertexArray(0);
	checkGLError();
	
	// Flashes shaders.
	_programFlashesId = createGLProgramFromStrings(ResourcesManager::getStringForShader("flashes_vert"), ResourcesManager::getStringForShader("flashes_frag"));
	
	glGenVertexArrays (1, &_vaoFlashes);
	glBindVertexArray(_vaoFlashes);
	// The first attribute will be the vertices positions.
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glVertexAttribDivisor(0, 0);
	// The second attribute will be the flags buffer.
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, _flagsBufferId);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, NULL);
	glVertexAttribDivisor(1, 1);
	// We load the indices data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
	
	// Flash texture loading.
	_texFlash = ResourcesManager::getTextureFor("flash");
	glUseProgram(_programFlashesId);
	glActiveTexture(GL_TEXTURE0);
	GLuint texUniID = glGetUniformLocation(_programFlashesId, "textureFlash");
	glUniform1i(texUniID, 0);
	glUseProgram(0);
	
	
	// Particles program.
	
	_programParticulesId = createGLProgramFromStrings(ResourcesManager::getStringForShader("particles_vert"), ResourcesManager::getStringForShader("particles_frag"));
	
	glGenVertexArrays (1, &_vaoParticles);
	glBindVertexArray(_vaoParticles);
	// The first attribute will be the vertices positions.
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glVertexAttribDivisor(0, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
	
	// Particles trajectories texture loading.
	_texParticles = ResourcesManager::getTextureFor("particles");
	glUseProgram(_programParticulesId);
	glActiveTexture(GL_TEXTURE0);
	GLuint texUniID1 = glGetUniformLocation(_programParticulesId, "textureParticles");
	glUniform1i(texUniID1, 0);
	
	glUseProgram(_programParticulesId);
	glActiveTexture(GL_TEXTURE1);
	GLuint texUniID2 = glGetUniformLocation(_programParticulesId, "lookParticles");
	glUniform1i(texUniID2, 1);
	
	// Pass texture size to shader.
	const glm::vec2 tsize = ResourcesManager::getTextureSizeFor("particles");
	GLuint texSizeID = glGetUniformLocation(_programParticulesId, "inverseTextureSize");
	glUniform2f(texSizeID, 1.0f/float(tsize[0]), 1.0f/float(tsize[1]));
	glUseProgram(0);
	
	// Keyboard setup.
	_programKeysId = createGLProgramFromStrings(ResourcesManager::getStringForShader("keys_vert"), ResourcesManager::getStringForShader("keys_frag"));
	glGenVertexArrays(1, &_vaoKeyboard);
	glBindVertexArray(_vaoKeyboard);
	// The first attribute will be the vertices positions.
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glVertexAttribDivisor(0, 0);
	// We load the indices data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
	glBindVertexArray(0);
	const GLuint uboLoc = glGetUniformBlockIndex(_programKeysId, "ActiveNotes");
	glUniformBlockBinding(_programKeysId, uboLoc, 0);

	// Prepare actives notes array.
	_actives = std::vector<float>(88, -1);
	// Particle systems pool.
	_particles = std::vector<Particles>(256);
}

void MIDIScene::setScaleAndMinorWidth(const float scale, const float minorWidth){
	glUseProgram(_programId);
	GLuint speedID = glGetUniformLocation(_programId, "mainSpeed");
	glUniform1f(speedID, scale);
	GLuint widthId = glGetUniformLocation(_programId, "minorsWidth");
	glUniform1f(widthId, minorWidth);
	glUseProgram(_programKeysId);
	GLuint widthId1 = glGetUniformLocation(_programKeysId, "minorsWidth");
	glUniform1f(widthId1, minorWidth);
	glUseProgram(0);
}

void MIDIScene::setParticlesParameters(const float speed, const float expansion){
	glUseProgram(_programParticulesId);
	GLuint id0 = glGetUniformLocation(_programParticulesId, "speedScaling");
	glUniform1f(id0, speed);
	GLuint id1 = glGetUniformLocation(_programParticulesId, "expansionFactor");
	glUniform1f(id1, expansion);
	glUseProgram(0);
}

void MIDIScene::updatesActiveNotes(double time, double delta){
	// Update the particle systems lifetimes.
	for(auto & particle : _particles){
		// Give a bit of a head start to the animation.
		particle.elapsed = (float(time) - particle.start + 0.25f) / particle.duration;
		// Disable old particles.
		if(float(time) >= particle.start + particle.duration){
			particle.note = -1;
			particle.duration = particle.start = particle.elapsed = 0.0f;
		}
	}

	// Move notes, time tracking, everything
	// delta_microseconds = 0 means, that we are on pause
	MidiEventListWithTrackId evs = _midi.Update((microseconds_t) (delta * 1000000));

	// These cycle is for keyboard updates (not falling keys)
	const size_t length = evs.size();
	for(size_t i = 0; i < length; ++i) {
	  const size_t &track_id = evs[i].first;
	  const MidiEvent &ev = evs[i].second;

	  if ((ev.Type() == MidiEventType_NoteOn || ev.Type() == MidiEventType_NoteOff) && ev.NoteNumber() >= 21 && ev.NoteNumber() <= 108) {
	    int vel = ev.NoteVelocity();
	    bool active = (vel > 0);
	    // Display pressed or released a key based on information from a MIDI-file.
	    // If this line is deleted, than no notes will be pressed automatically.
	    // It is not related to falling notes.

		if(active){
			// try to find note in set of remaining notes
			const TranslatedNote* _note = nullptr;
			int j = 0;
			for (auto& note : _notes) {
				if (note.note_id == ev.NoteNumber()) {
					_note = &note;
					break;
				}
				if (j >= 200)
					break; // give up
				j++;
			}
			_actives[ev.NoteNumber() - 21] = _note != nullptr ? _note->track_id : 0;

			// Find an available particles system and update it with the note parameters.
			for(auto & particle : _particles){
				if(particle.note < 0){
					// Update with new note parameter.
					float duration = _note != nullptr ? (_note->end - _note->start) / 1000000.0f : 1.0f;
					particle.duration = (std::max)(duration*2.0f, duration + 1.2f);
					particle.start = time;
					particle.note = ev.NoteNumber() - 21;
					particle.elapsed = 0.0f;
					break;
				}
			}
		} else
			_actives[ev.NoteNumber() - 21] = -1;
	  }
	}

    // Delete notes that are finished playing (and are no longer available to hit)
	microseconds_t cur_time = _midi.GetSongPositionInMicroseconds();
    TranslatedNoteSet::iterator i = _notes.begin();
	while (i != _notes.end()) {
		TranslatedNoteSet::iterator note = i++;
		const microseconds_t window_end = note->start + 100000; // threshold
		if (note->start > cur_time)
			break;
		if (note->end < cur_time && window_end < cur_time)
			_notes.erase(note);
	}
}

void MIDIScene::reset(float prerollTime) {
	for (auto & particle : _particles) {
		particle.note = -1;
		particle.duration = particle.start = particle.elapsed = 0.0f;
	}
	_midi.Reset(prerollTime * 1000000, 0);
	_notes = _midi.Notes();
	_actives = std::vector<float>(88, -1);
}

void MIDIScene::drawParticles(float time, const glm::vec2 & invScreenSize, const State::ParticlesState & state, bool prepass){

	glEnable(GL_BLEND);
	glUseProgram(_programParticulesId);
	
	// Common uniforms values.
	GLuint screenId = glGetUniformLocation(_programParticulesId, "inverseScreenSize");
	GLuint timeId = glGetUniformLocation(_programParticulesId, "time");
	GLuint durationId = glGetUniformLocation(_programParticulesId, "duration");
	glUniform2fv(screenId,1, &(invScreenSize[0]));
	glUniform1f(timeId,time);
	glUniform1f(timeId,0.0);//wut?

	// Variable uniforms.
	GLuint globalShiftId = glGetUniformLocation(_programParticulesId, "globalId");
	GLuint scaleId = glGetUniformLocation(_programParticulesId, "scale");
	GLuint colorId = glGetUniformLocation(_programParticulesId, "baseColor");
	
	// Prepass : bigger, darker particles.
	const glm::vec3 & pcol = state.color;
	if(prepass){
		glUniform1f(scaleId, state.scale*2.0f);
		glUniform3f(colorId, 0.6f*pcol[0], 0.6f*pcol[1], 0.6f*pcol[2]);
	} else {
		glUniform1f(scaleId, state.scale);
		glUniform3f(colorId, 1.6f*pcol[0], 1.6f*pcol[1], 1.6f*pcol[2]);
	}
	
	// Particles trajectories texture.
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _texParticles);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D_ARRAY, state.tex);
	GLuint texCountId = glGetUniformLocation(_programParticulesId, "texCount");
	glUniform1i(texCountId, state.texCount);

	// Select the geometry.
	glBindVertexArray(_vaoParticles);
	// For each activ particles system, draw it with the right parameters.
	for(const auto & particle : _particles){
		if(particle.note >= 0){
			glUniform1i(globalShiftId, particle.note);
			glUniform1f(timeId, particle.elapsed);
			glUniform1f(durationId, particle.duration);
			glDrawElementsInstanced(GL_TRIANGLES, int(_primitiveCount), GL_UNSIGNED_INT, (void*)0, state.count);
		}
	}
	
	glBindVertexArray(0);
	glUseProgram(0);
	glDisable(GL_BLEND);

}

void MIDIScene::drawNotes(float time, const glm::vec2 & invScreenSize, const glm::vec3 & primaryColor, const glm::vec3 & secondaryColor, bool prepass){
	
	glUseProgram(_programId);
	
	// Uniforms setup.
	GLuint screenId = glGetUniformLocation(_programId, "inverseScreenSize");
	GLuint timeId = glGetUniformLocation(_programId, "time");
	GLuint primaryColorId = glGetUniformLocation(_programId, "primaryColor");
	GLuint secondaryColorId = glGetUniformLocation(_programId, "secondaryColor");
	glUniform2fv(screenId,1, &(invScreenSize[0]));
	glUniform1f(timeId,time);
	if(prepass){
		glUniform3f(primaryColorId, 0.6f*primaryColor[0], 0.6f*primaryColor[1], 0.6f*primaryColor[2]);
		glUniform3f(secondaryColorId, 0.6f*secondaryColor[0], 0.6f*secondaryColor[1], 0.6f*secondaryColor[2]);
	} else {
		glUniform3fv(primaryColorId, 1, &(primaryColor[0]));
		glUniform3fv(secondaryColorId, 1, &(secondaryColor[0]));
	}
	
	
	// Draw the geometry.
	glBindVertexArray(_vao);
	glDrawElementsInstanced(GL_TRIANGLES, int(_primitiveCount), GL_UNSIGNED_INT, (void*)0, GLsizei(_notesCount));

	glBindVertexArray(0);
	glUseProgram(0);
	
}

void MIDIScene::drawFlashes(float time, const glm::vec2 & invScreenSize, const glm::vec3 & primaryColor, const glm::vec3 & secondaryColor, float userScale){
	
	// Need alpha blending.
	glEnable(GL_BLEND);
	
	// Update the flags buffer accordingly.
	glBindBuffer(GL_ARRAY_BUFFER, _flagsBufferId);
	glBufferSubData(GL_ARRAY_BUFFER, 0, _actives.size()*sizeof(float) ,&(_actives[0]));
	
	glUseProgram(_programFlashesId);
	
	// Uniforms setup.
	GLuint screenId1 = glGetUniformLocation(_programFlashesId, "inverseScreenSize");
	GLuint timeId1 = glGetUniformLocation(_programFlashesId, "time");
	GLuint primaryColorId = glGetUniformLocation(_programFlashesId, "primaryColor");
	GLuint secondaryColorId = glGetUniformLocation(_programFlashesId, "secondaryColor");
	GLuint scaleId = glGetUniformLocation(_programFlashesId, "userScale");
	glUniform2fv(screenId1,1, &(invScreenSize[0]));
	glUniform1f(timeId1,time);
	glUniform3fv(primaryColorId, 1, &(primaryColor[0]));
	glUniform3fv(secondaryColorId, 1, &(secondaryColor[0]));
	glUniform1f(scaleId,userScale);
	// Flash texture.
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _texFlash);
	
	// Draw the geometry.
	glBindVertexArray(_vaoFlashes);
	glDrawElementsInstanced(GL_TRIANGLES, int(_primitiveCount), GL_UNSIGNED_INT, (void*)0, 88);
	
	glBindVertexArray(0);
	glUseProgram(0);
	glDisable(GL_BLEND);
	
}

void MIDIScene::drawKeyboard(float, const glm::vec2 & invScreenSize, const glm::vec3 & keyColor, const glm::vec3 & primaryColor, const glm::vec3 & secondaryColor, bool highlightKeys) {
	// Upload active keys data.
	glBindBuffer(GL_UNIFORM_BUFFER, _uboKeyboard);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, _actives.size() * sizeof(float), &(_actives[0]));
	//glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glUseProgram(_programKeysId);

	// Uniforms setup.
	const GLuint screenId1 = glGetUniformLocation(_programKeysId, "inverseScreenSize");
	const GLuint colorId = glGetUniformLocation(_programKeysId, "keysColor");
	const GLuint primaryColorId = glGetUniformLocation(_programKeysId, "primaryColor");
	const GLuint secondaryColorId = glGetUniformLocation(_programKeysId, "secondaryColor");
	const GLuint highId = glGetUniformLocation(_programKeysId, "highlightKeys");
	glUniform2fv(screenId1, 1, &(invScreenSize[0]));
	glUniform3fv(colorId, 1, &(keyColor[0]));
	glUniform3fv(primaryColorId, 1, &(primaryColor[0]));
	glUniform3fv(secondaryColorId, 1, &(secondaryColor[0]));
	glUniform1i(highId, int(highlightKeys));

	glBindBuffer(GL_UNIFORM_BUFFER, _uboKeyboard);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, _uboKeyboard);

	// Draw the geometry.
	glBindVertexArray(_vaoKeyboard);
	glDrawElements(GL_TRIANGLES, int(_primitiveCount), GL_UNSIGNED_INT, (void*)0);

	glBindVertexArray(0);
	glUseProgram(0);
}

void MIDIScene::clean(){
	glDeleteVertexArrays(1, &_vao);
	glDeleteVertexArrays(1, &_vaoFlashes);
	glDeleteVertexArrays(1, &_vaoParticles);
	glDeleteProgram(_programId);
	glDeleteProgram(_programFlashesId);
	glDeleteProgram(_programParticulesId);
}


