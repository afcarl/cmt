import sys
import unittest

sys.path.append('./code')

from mcm import MCGSM
from mcm import random_select
from mcm import generate_data_from_image, sample_image
from mcm import generate_data_from_video, sample_video
from mcm import LinearTransform
from numpy import *
from numpy import max
from numpy.random import *

class ToolsTest(unittest.TestCase):
	def test_random_select(self):
		# select all elements
		self.assertTrue(set(random_select(8, 8)) == set(range(8)))

		# n should be larger than k
		self.assertRaises(Exception, random_select, 10, 4)



	def test_generate_data_from_image(self):
		xmask = asarray([
			[1, 1],
			[1, 0]], dtype='bool')
		ymask = asarray([
			[0, 0],
			[0, 1]], dtype='bool')

		img = asarray([
			[1., 2.],
			[3., 4.]])

		inputs, outputs = generate_data_from_image(img, xmask, ymask, 1)

		self.assertLess(max(abs(inputs - [[1.], [2.], [3.]])), 1e-10)
		self.assertLess(max(abs(outputs - [[4.]])), 1e-10)

		inputs, outputs = generate_data_from_image(randn(512, 512), xmask, ymask, 100)

		self.assertEqual(inputs.shape[0], 3)
		self.assertEqual(inputs.shape[1], 100)
		self.assertEqual(outputs.shape[0], 1)
		self.assertEqual(outputs.shape[1], 100)

		inputs, outputs = generate_data_from_image(randn(512, 512, 2), xmask, ymask, 100)

		self.assertEqual(inputs.shape[0], 6)
		self.assertEqual(inputs.shape[1], 100)
		self.assertEqual(outputs.shape[0], 2)
		self.assertEqual(outputs.shape[1], 100)

		# multi-channel masks
		xmask = dstack([
			asarray([
				[1, 1],
				[1, 0]], dtype='bool'),
			asarray([
				[1, 1],
				[1, 0]], dtype='bool')])
		ymask = dstack([
			asarray([
				[0, 0],
				[0, 1]], dtype='bool'),
			asarray([
				[0, 0],
				[0, 1]], dtype='bool')])

		inputs, outputs = generate_data_from_image(randn(512, 512, 2), xmask, ymask, 100)

		self.assertEqual(inputs.shape[0], 6)
		self.assertEqual(inputs.shape[1], 100)
		self.assertEqual(outputs.shape[0], 2)
		self.assertEqual(outputs.shape[1], 100)

		# invalid masks due to overlap
		xmask = asarray([
			[1, 1],
			[1, 1]], dtype='bool')
		ymask = asarray([
			[0, 0],
			[0, 1]], dtype='bool')

		self.assertRaises(Exception, generate_data_from_image, img, xmask, ymask, 1)



	def test_generate_data_from_video(self):
		xmask = dstack([
			asarray([
				[1, 1],
				[1, 1]], dtype='bool'),
			asarray([
				[1, 1],
				[1, 0]], dtype='bool')])
		ymask = dstack([
			asarray([
				[0, 0],
				[0, 0]], dtype='bool'),
			asarray([
				[0, 0],
				[0, 1]], dtype='bool')])

		inputs, outputs = generate_data_from_video(randn(512, 512, 5), xmask, ymask, 100)

		self.assertEqual(inputs.shape[0], 7)
		self.assertEqual(inputs.shape[1], 100)
		self.assertEqual(outputs.shape[0], 1)
		self.assertEqual(outputs.shape[1], 100)

		# invalid masks due to overlap
		xmask = dstack([
			asarray([
				[1, 1],
				[1, 1]], dtype='bool'),
			asarray([
				[1, 1],
				[1, 0]], dtype='bool')])
		ymask = dstack([
			asarray([
				[0, 0],
				[0, 1]], dtype='bool'),
			asarray([
				[0, 0],
				[0, 1]], dtype='bool')])

		self.assertRaises(Exception, generate_data_from_video, randn(512, 512, 5), xmask, ymask, 1)



	def test_sample_image(self):
		xmask = asarray([
			[1, 1],
			[1, 0]], dtype='bool')
		ymask = asarray([
			[0, 0],
			[0, 1]], dtype='bool')

		img_init = asarray([
			[1., 2.],
			[3., 4.]])

		model = MCGSM(3, 1)

		img_sample = sample_image(img_init, model, xmask, ymask)

		# only the bottom right-pixel should have been replaced
		self.assertLess(max(abs((img_init - img_sample).ravel()[:3])), 1e-10)



	def test_sample_video(self):
		xmask = dstack([
			asarray([
				[1, 1, 1],
				[1, 1, 1],
				[1, 1, 1]], dtype='bool'),
			asarray([
				[1, 1, 1],
				[1, 0, 0],
				[0, 0, 0]], dtype='bool')])
		ymask = dstack([
			asarray([
				[0, 0, 0],
				[0, 0, 0],
				[0, 0, 0]], dtype='bool'),
			asarray([
				[0, 0, 0],
				[0, 1, 0],
				[0, 0, 0]], dtype='bool')])

		model = MCGSM(13, 1)

		video_init = randn(64, 64, 5)
		video_sample = sample_video(video_init, model, xmask, ymask)

		# the first frame should be untouched
		self.assertLess(max(abs(video_init[:, :, 0] - video_sample[:, :, 0])), 1e-10)



if __name__ == '__main__':
	unittest.main()