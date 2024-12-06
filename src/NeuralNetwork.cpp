/*
 */
#include <NeuralNetwork.hpp>
#include <Logger.hpp>
using namespace nnpp;
/*
 */
NeuralNetwork::NeuralNetwork(const std::vector<unsigned long> &layerSizes)
{
	auto layerSizesSize = layerSizes.size();
	for (unsigned long layerIndex = 0; layerIndex < layerSizesSize; ++layerIndex)
	{
		unsigned long numberOfInputs = (layerIndex == 0) ? layerSizes[0] : layerSizes[layerIndex - 1];
		layers.push_back({layerSizes[layerIndex], numberOfInputs});
	}
};
/*
 */
void NeuralNetwork::print()
{
	auto layersSize = layers.size();
	for (unsigned long layerIndex = 0; layerIndex < layersSize; layerIndex++)
	{
		logger(Logger::Blank, "Layer: " + std::to_string(layerIndex));
		auto &layer = layers[layerIndex];
		auto neuronsSize = layer.neurons.size();
		for (unsigned long neuronIndex = 0; neuronIndex < neuronsSize; neuronIndex++)
		{
			auto &neuron = layer.neurons[neuronIndex];
			logger(Logger::Blank,
				"\tNeuron: " + std::to_string(neuronIndex) +
					", inputValue: " + std::to_string(neuron.inputValue) +
					", outputValue: " +  std::to_string(neuron.outputValue) +
					", bias: " +  std::to_string(neuron.bias) +
					", gradient: " + std::to_string(neuron.gradient)
			);
		}
	}
}
/*
 */
const long double sigmoid(const long double &x)
{
	return 1.0 / (1.0 + exp(-x));
};
/*
 */
void NeuralNetwork::feedforward(const std::vector<long double> &inputValues)
{
	// Assign input values to the first layer
	for (size_t i = 0; i < layers[0].neurons.size(); ++i)
	{
		layers[0].neurons[i].outputValue = inputValues[i];
	}
	// Forward propagate through subsequent layers
	for (size_t layerIndex = 1; layerIndex < layers.size(); ++layerIndex)
	{
		auto &prevLayer = layers[layerIndex - 1];
		for (auto &neuron : layers[layerIndex].neurons)
		{
			neuron.inputValue = 0.0; // Reset the input value
			for (unsigned long n = 0; n < prevLayer.neurons.size(); ++n)
			{
				// Accumulate the weighted input values
				neuron.inputValue += prevLayer.neurons[n].outputValue * neuron.weights[n];
			}
			// Add the bias and apply the activation function
			neuron.inputValue += neuron.bias;
			neuron.outputValue = sigmoid(neuron.inputValue);
		}
	}
};
/*
 */
const long double sigmoidDerivative(const long double &x)
{
	return x * (1.0 - x);
}
/*
 */
void NeuralNetwork::backpropagate(const std::vector<long double> &targetValues)
{
	// Calculate gradients for the output layer
	Layer &outputLayer = layers.back();
	for (size_t i = 0; i < outputLayer.neurons.size(); ++i)
	{
		long double delta = targetValues[i] - outputLayer.neurons[i].outputValue;
		outputLayer.neurons[i].gradient = delta * sigmoidDerivative(outputLayer.neurons[i].outputValue);
	}

	// Calculate gradients for the hidden layers (in reverse order)
	for (size_t layerIndex = layers.size() - 2; layerIndex > 0; --layerIndex)
	{
		Layer &hiddenLayer = layers[layerIndex];
		Layer &nextLayer = layers[layerIndex + 1];

		for (size_t neuronIndex = 0; neuronIndex < hiddenLayer.neurons.size(); ++neuronIndex)
		{
			long double error = 0.0;
			for (size_t nextNeuronIndex = 0; nextNeuronIndex < nextLayer.neurons.size(); ++nextNeuronIndex)
			{
				error += nextLayer.neurons[nextNeuronIndex].weights[neuronIndex] * nextLayer.neurons[nextNeuronIndex].gradient;
			}
			hiddenLayer.neurons[neuronIndex].gradient = error * sigmoidDerivative(hiddenLayer.neurons[neuronIndex].outputValue);
		}
	}

	// Update weights and biases for all layers (except input layer)
	auto layersSize = layers.size();
	for (size_t layerIndex = 1; layerIndex < layersSize; ++layerIndex)
	{
		Layer &layer = layers[layerIndex];
		Layer &prevLayer = layers[layerIndex - 1];

		for (Neuron &neuron : layer.neurons)
		{
			auto weightsSize = neuron.weights.size();
			for (size_t w = 0; w < weightsSize; ++w)
			{
				neuron.weights[w] += learningRate * neuron.gradient * prevLayer.neurons[w].outputValue;
			}
			neuron.bias += learningRate * neuron.gradient;
		}
	}
};
/*
 */
const std::vector<long double> NeuralNetwork::getOutputs() const
{
	auto &lastLayer = layers.back();
	std::vector<long double> outputs;
	auto neuronsSize = lastLayer.neurons.size();
	for (unsigned long neuronIndex = 0; neuronIndex < neuronsSize; neuronIndex++)
	{
		outputs.push_back(lastLayer.neurons[neuronIndex].outputValue);
	}
	return outputs;
};
/*
 */